;;; stats.lsp
;;; =========

(setq *pgm-version* "$Revision: 3.0.2.2 $")
(setq *pgm-date*    "$Date: 2002/12/02 23:46:26 $")
(setq *rcsid*       "$Id: stats.lsp,v 3.0.2.2 2002/12/02 23:46:26 johann Exp $")


;;; CHANGES
;;;   2001.12.08 2.0: suppress calculation of standard deviation of
;;;     error of folds for experiments with only one fold
;;;   2001.08.28 1.9: calculation of pvalue for mcnemar test changed:
;;;     added treatment of special case where both parameters are equal.
;;;     in that case, dont do the continuity correction!
;;;   2001.08.05 1.8: added measures StdErrOfError and StdDevOfError
;;;     the standard error (standarddev(e_i)/sqrt(n)) of the n fold errors e_i,
;;;     and the standard deviation of the n fold errors e_i, respectively
;;;   2001.07.05 1.7: bug correction - we didnt treat the
;;;     case specially where the standard deviation of the differences
;;;     is zero -- in that case we return "?"
;;;   2001.07.05 1.6:
;;;      added pvalMcNemar because the old measurement p-val_McNemar
;;;      is by mistake 1-correct value.
;;;     - added new measure pvalPairedTTest for classification
;;;       to compare the mean of the fold errors for each
;;;       pair of algorithms with a paired t-test. H1 is that
;;;       means are different.
;;;   2000.11.22 1.5: Bug correction: wrong calculation of NMAD;
;;;      added calculation of RSquared (correlation coefficient)
;;;   2000.11.15 1.4: Bug correction: read in class labels preserving
;;;      case!
;;;   2000.10.19 1.3: bug correction - correct wrong statistics
;;;      incorrectly printed (but calculation and mcnemar ok)
;;;   2000.08.08 1.2: bug correction use reduce instead of apply
;;;      to avoid stack overflow errors
;;;   2000.07.03 1.1: bug correction: fold info for classification
;;;      showed accuracy, not error.

;;; NOTE: only works with XLISPSTAT.
;;;   tested only with XLISPSTAT 3.52.13 (beta) on Sparc/Solaris
;;; XLISPSTAT-specific functions have been annotated with XLS!
;;; to allow reimplementation in LISP for other LISPs

;;; data structures:
;;;   We store the predictions and targets as LISP converts them
;;;   when reading from the file. This goes for both classification
;;;   and regression problems, i.e for strings and numbers.
;;;   Subsequently, for classification, eql is used to check
;;;   for equality, for regression, the difference is calculated.
;;;
;;;   All sets of data items, differences etc. are stored in lists.

;;;   For each FOLD, we store the current targets in *fold-targets*
;;;   and the predictions for each algorithm in the hash *fold-predictions*

;;;   The set of *all* targets and predictions is stored in *all-targets*
;;;   and the assoc *all-predictions*

;;;   to speed up things, the data items are pushed into the all - lists,
;;;   i.e. they are stored in reverse order! (but order is assumed not
;;;   to matter anyway

;;; NOTE repeats are currently handled so that folds of different
;;; repeats are handled like folds from the same repeat.
;;; The program just checks that the number of files for the
;;; targets and each algorithm is the same as (folds * repeats)
;;; The data from each fold/repeat is called "chunk"

;;; the following globals should get set from the pelr script for
;;; a particular experiment (even if defaults are ok)
(setq *algtype* :regression)
(setq *nr-folds* 0)   ;; number of folds
(setq *nr-rep*   1)   ;; number of repeats
(setq *nr-algs*  0)   ;; number of algorithms
(setq *verbose* nil)
(setq *debug* nil)
;;; -----

(defun init-all-data ()
  "clear/init all data"
  (setq *nr-chunks-targets* 0)   ;; number of chunks read for target files

  (setq *chunk-targets* nil)
  (setq *chunk-predictions* (make-hash-table :test #'equal))
  (setq *chunk-errors* (make-hash-table :test #'equal))
  
  (setq *all-targets* nil)
  (setq *all-predictions* (make-hash-table :test #'equal ))
  (setq *all-algerror* (make-hash-table :test #'equal))
  (setq *nr-chunks-alg* (make-hash-table :test #'equal ))

  (setq *log-output* *standard-output*)
  (setq *stats-output* *standard-output*)
 
  )

(init-all-data)

(defun show-info ()
  "show info about the current data ... for testing mainly"
  (format *log-output* "number of target chunks read:   ~7d~%" *nr-chunks-targets*)
  (format *log-output* "number of targets for chunk:    ~7d~%" (length *chunk-targets*))
  (format *log-output* "number of targets in all:       ~7d~%" (length *all-targets*))
  (format *log-output* "Algorithms: ~a~%" (get-algorithms *all-predictions*))
  )

;;; !!!!
(defun set-logfile (filename)
  "redirect log information to specified file"
  (setq *log-output* (open filename :direction :output))
  (setq *error-output* *log-output*)
  )

(defun set-statsfile (filename)
  "redirect output of statistics to specified file"
  (setq *stats-output* (open filename :direction :output))
  )

(defun get-algorithms (hash)
  (let (algs)
    (maphash #'(lambda (x y) (push x algs)) hash)
    (setq algs (sort algs #'(lambda (x y) (string< (as-string x) (as-string y)))))
    (mapcar #'as-string algs)
    ))

;; all interface functions (i.e. those called from the perl-generated file)
;; are marked with ;; !!!
(defun read-file (filename)
  "read a file and return a list of values"
  (let ((oldreadtablecase *READTABLE-CASE*)
	ret)
  ;;; XLS! read-data-file
    (setq *READTABLE-CASE* :PRESERVE)
    (setq ret (read-data-file filename))
    (setq *READTABLE-CASE* OLDREADTABLECASE)
    (when *debug* (print ret))
    ret))

;;; !!!
(defun read-chunk-targets (filename)
  "read and store the targets for a chunk"
  (let (n)
    (setq *chunk-algerror* (make-hash-table :test #'equal))
    (setq *chunk-targets* (read-file filename))
    (setq n (length *chunk-targets*))
    (unless (> n 0)
      (format *log-output* "empty target file: ~a~%" filename)
      (error "unrecoverable error, exiting!")
      )
    ;; increment the number of chunks for the targets
    (incf *nr-chunks-targets*)
    (setq *all-targets* (append *chunk-targets* *all-targets*))
    )
  )

;;; !!!
(defun read-chunk-predictions (filename algname)
  "read and store the predictions for a chunk. this must be
   called after a read-chunk-targets so the number of predictions
   can be checked. If they dont match, we flag the algorithm as
   being in error and will not calculate error measures for it."
  (block func
  (let ((alg (as-hashkey algname))
	preds n m)
    (setq preds (read-file filename))
    (setq n (length *chunk-targets*))
    (setq m (length preds))
    (unless (= n m)
      (format *log-output*
	      "read-chunk-predictions: Number of predictions for ~a (~d) doesnt match targets (~d) (file: ~a/alg: ~a)~%"
	      alg m n filename algname)
      (setf (gethash alg *all-algerror*) t)
      (setf (gethash alg *chunk-algerror*) t)
      (return-from func)
      )
    (setf (gethash alg *chunk-predictions*) preds)
    (unless (gethash alg *all-algerror*)
      (setf (gethash alg *all-predictions*)
	    (append preds (gethash alg *all-predictions*))))
    (if (gethash alg *nr-chunks-alg*)
	(incf (gethash alg *nr-chunks-alg*))
	(setf (gethash alg *nr-chunks-alg*) 1))
    )))

;;; !!!
(defun drop-chunk-predictions (filename algname)
  "just flag as error"
  (block func
  (let ((alg (as-hashkey algname))
	preds n m)
   (setf (gethash alg *all-algerror*) t)
   (setf (gethash alg *chunk-algerror*) t)
   )))
;;; REGRESSION ==================

(defun regr-stats-chunk-alg (alg &optional rep fold)
  "Calculate the regression statistics for current fold for
   the given algorithm."
  (block func
  (let (diffs preds (alg (as-hashkey alg)))
    (when (gethash alg *chunk-algerror*)
      (return-from func))
    (setq preds (gethash alg *chunk-predictions*))
    (unless preds
      (format *log-output*
	      "Didnt find fold predictions for algorithm:  ~a~%" alg)
      (setf (gethash alg *all-algerror*) t)
      (return-from func)
      )
    (setq n (length *chunk-targets*))
    (setq m (length preds))
    (unless (= n m)
      (format *log-output*
	      "regr-stats-chunk-alg: Number of targets (~d) and predictions (~d) different (alg: ~a/rep: ~d/fold: ~d)~%"  n m alg rep fold)
      (setf (gethash alg *all-algerror*) t)
      (return-from func)
      )
    (setq diffs (- *chunk-targets* preds))
    (when *debug*
      (format *log-output* "Calculating chunk ~d/~d regression stats for ~a~%"
	      rep fold alg))
    (regr-calc-stats preds *chunk-targets* diffs  alg rep fold)
    )))



(defun regr-stats-all-alg (alg)
  "Calculate the regression statistics for all predictions of
   given algorithm."
  (block func
  (let (diffs preds (alg (as-hashkey alg)))
    (when (gethash alg *all-algerror*)
      (format *log-output*
	      "Previous problem with algorithm ~a, not calculating overall errors~%"
	      alg)
      (return-from func))
    (setq preds (gethash alg *all-predictions*))
    (unless preds
      (error "Didnt find predictions for algorithm: ~a~%" alg))
    (setq n (length *all-targets*))
    (setq m (length preds))
    (unless (= n m)
      (error "regr-stats-all-alg: Number of targets (~d) and predictions (~d) different (alg: ~a)~%" n m alg))
    (setq diffs (- *all-targets* preds))
    (when *verbose*
      (format *log-output* "Calculating all regression stats for ~a~%"
	      alg))
    (regr-calc-stats preds *all-targets* diffs alg nil nil)
    )))

;;; !!!
(defun regr-stats-chunk (&optional rep fold)
  "Calculate and show all regression statistics for all the
   algorithms for which we do have data"
  (dolist (alg (get-algorithms *chunk-predictions*))
    (regr-stats-chunk-alg alg rep fold)
      ))
  
;;; !!!
(defun regr-stats-all ()
  "Calculate and show all regression statistics for all the
   algorithms for which we do have data"
  (dolist (alg (get-algorithms *all-predictions*))
    (regr-stats-all-alg alg)
    ))
  
  

(defun regr-calc-stats (preds targets diffs  alg rep fold)
  "Calculate regression statistics. Alg rep and fold are
   given for printing only. if fold and/or rep is nil,
   that output is suppressed"
  (let (se n m ae sse mse rmse nmse sad mad sadtm nmad median ssto rsqr)
    (setq n (length diffs))
    (setq median_t (median targets))
    (setq se (sqr diffs))
    (setq ae (abs diffs))     ;; absolute errors
    (setq sse (reduce #'+ se)) ;; sum squared errors
    (setq mse (/ sse n))      ;; mean squared error
    (setq rmse (sqrt mse))    ;; root mean squared error
    (setq ssto (reduce #'+ (sqr (- (mean targets) targets))))
    (if (equal ssto 0.0)
	(setq nmse "?")
	(setq nmse (/ sse ssto)))    ;; normalized mean squared error
    (setq sad (reduce #'+ ae))  ;; sum absolute deviation
    (setq mad (/ sad n))  ;; mean absolute deviation
    (setq sadtm (reduce #'+ (abs (- targets median_t)))) ;; sum abs dev of target median
    (if (equal sadtm 0.0)
	(setq nmad "?")
	(setq nmad (/ sad sadtm))) ;; normalized mean absolute deviation
    (if (equal ssto 0.0)
	(setq rsqr "?")
	(setq rsqr (- 1.0 (/ sse ssto))))
    (setq text (format nil " ~a~a~a"
		       alg
		       (if rep (format nil " ~d" rep) "")
		       (if fold (format nil " ~d" fold) "")))
    (format *stats-output* "ErrorSSE~a: ~f~%" text sse)
    (format *stats-output* "ErrorMSE~a: ~f~%" text mse)
    (format *stats-output* "ErrorRMSE~a: ~f~%" text rmse)
    (format *stats-output* "ErrorNMSE~a: ~f~%" text nmse)
    (format *stats-output* "ErrorMAD~a: ~f~%" text mad)
    (format *stats-output* "ErrorNMAD~a: ~f~%" text nmad)
    (format *stats-output* "RSquare~a: ~f~%" text rsqr)
    ))

;;; !!!
(defun regr-stats-pairs ()
  "Calculate and show all  statistics for 
   algorithm pairings for which we do have data"
  (let ((algs  (get-algorithms *all-predictions*))
	algs2)
  (setq algs2 algs)
  (dolist (alg1 algs)
    (setq algs2 (remove alg1 algs2))
    (dolist (alg2 algs2)
      (regr-stats-pair alg1 alg2)
      ))
  ))
  
(defun regr-stats-pair (alg1 alg2)
  "statistics for comparison of two regression algorithms"
  (block func
    (let (diffs1 preds1
	  diffs2 preds2
	  d d-mean d-var u
	  (alg1 (as-hashkey alg1))
	  (alg2 (as-hashkey alg2))
	  )
      (when *verbose*
	(format *log-output* "Calculating PAIR stats for ~a and ~a~%"
		alg1 alg2))
      (when (gethash alg1 *all-algerror*)
	(return-from func))
      (when (gethash alg2 *all-algerror*)
	(return-from func))
      (setq preds1 (gethash alg1 *all-predictions*))
      (unless preds1
	(error "Didnt find predictions for algorithm: ~a~%" alg1))
      (setq preds2 (gethash alg2 *all-predictions*))
      (unless preds2
	(error "Didnt find predictions for algorithm: ~a~%" alg2))
      (setq diffs1 (- *all-targets* preds1))
      (setq diffs2 (- *all-targets* preds2))
      (setq n (length diffs1))
      (setq d (- (sqr diffs1) (sqr diffs2)))
      (setq d-mean (mean d))
      (setq d-var  (sqr (standard-deviation d)))
      (if (= 0.0 d-var)
       (format *stats-output* "p-MeanDiffZero ~a ~a: ~,5f~%"
	       alg1 alg2 1.0 )
       (progn
	 (setq u (/ (* (sqrt (length d)) d-mean) d-var))
	 (format *stats-output* "p-MeanDiffZero ~a ~a: ~,5f~%"
		 alg1 alg2 (normal-cdf u) )
	 ))
      )))

;;; CLASSIFICATION ==================

(defun class-stats-chunk-alg (alg &optional rep fold)
  "Calculate the classification statistics for current fold for
   the given algorithm."
  (block func
  (let (diffs preds (alg (as-hashkey alg)))
    (when (gethash alg *chunk-algerror*)
      (return-from func))
    (setq preds (gethash alg *chunk-predictions*))
    (unless preds
      (format *log-output*
	      "Didnt find fold predictions for algorithm:  ~a~%" alg)
      (setf (gethash alg *all-algerror*) t)
      (return-from func)
      )
    (setq n (length *chunk-targets*))
    (setq m (length preds))
    (unless (= n m)
      (format *log-output*
	      "class-stats-chunk-alg: Number of targets (~d) and predictions (~d) different (alg: ~a/rep: ~d/fold: ~d)~%" n m alg rep fold)
      (setf (gethash alg *all-algerror*) t)
      (return-from func)
      )
    (setq diffs (mapcar #'(lambda (x y) (if (eql x y) 0 1))
			*chunk-targets* preds))
    (class-calc-stats diffs *chunk-targets* alg rep fold)
    )))


(defun class-stats-all-alg (alg)
  "Calculate the classification statistics for all predictions of
   given algorithm."
  (block func
  (let (diffs preds (alg (as-hashkey alg)))
    (when (gethash alg *all-algerror*)
      (format *log-output*
	      "Previous problem with algorithm ~a, not calculating overall errors~%"
	      alg)
      (return-from func))
    (setq preds (gethash alg *all-predictions*))
    (unless preds
      (error "Didnt find predictions for algorithm: ~a~%" alg))
    (setq n (length *all-targets*))
    (setq m (length preds))
    (unless (= n m)
      (error "class-stats-all-alg: Number of targets (~d) and predictions (~d) different (alg: ~a)~%" n m alg))
    (setq diffs (mapcar #'(lambda (x y) (if (eql x y) 0 1))
			*all-targets* preds))
    (class-calc-stats diffs *all-targets* alg nil nil)
    )))

(defun class-stats-pair (alg1 alg2)
  "Calculate the classification statistics for a pair of algs."
  (block func
  (let (diffs1
	diffs2
	preds1 preds2
	(alg1 (as-hashkey alg1))
	(alg2 (as-hashkey alg2))
	t1
	tmp
	(errs1 nil)
	(errs2 nil)
	)
    (when *verbose*
      (format *log-output* "Calculating pair stats for ~a and ~a~%"
	      alg1 alg2))
    (when (gethash alg1 *all-algerror*)
      (return-from func))
    (when (gethash alg2 *all-algerror*)
      (return-from func))
    (setq preds1 (gethash alg1 *all-predictions*))
    (unless preds1
      (error "Didnt find predictions for algorithm: ~a~%" alg1))
    (setq preds2 (gethash alg2 *all-predictions*))
    (unless preds2
      (error "Didnt find predictions for algorithm: ~a~%" alg2))
    (setq diffs1 (mapcar #'(lambda (x y) (if (eql x y) 1 0))
			 *all-targets* preds1))
    (setq diffs2 (mapcar #'(lambda (x y) (if (eql x y) 1 0))
			 *all-targets* preds2))
    ;; now we check the number of cases where alg1 is right but alg2 wrong
    ;; and also the number of cases where alg1 is wrong but alg2 right
    (setq rightwrong
	  (reduce #'+
		 (mapcar #'(lambda (x y) (if (and (= 0 x) (= 1 y)) 1 0))
			 diffs1 diffs2)))
    (setq wrongright
	  (reduce #'+
		 (mapcar #'(lambda (x y) (if (and (= 1 x) (= 0 y)) 1 0))
			 diffs1 diffs2)))
    (format *stats-output* "Correct-Wrong ~a ~a: ~d~%" alg1 alg2 wrongright)
    (format *stats-output* "Wrong-Correct ~a ~a: ~d~%" alg1 alg2 rightwrong)
    (setq t1 (mcnemar rightwrong wrongright))
    (format *stats-output* "pvalMcNemar ~a ~a: ~,5f~%"
	    alg1 alg2 t1)
    (format *stats-output* "p-Val_McNemar ~a ~a: ~,5f~%"
	    alg1 alg2 (- 1.0 t1))
    (format *log-output* "Here ~a ~a ~%" *nr-rep* *nr-fold*)
    (dotimes (rep *nr-rep*)
      (dotimes (fold *nr-fold*)
	(format *log-output* "Doing ~a ~a ~%" rep fold)
	(setq tmp (gethash (list alg1 rep fold) *chunk-errors*))
	(unless tmp
	  (format *log-output* "No error measure found for alg ~a, rep ~a, fold ~a~%" alg1 rep fold)
	  )
	(push tmp errs1)
	(setq tmp (gethash (list alg2 rep fold) *chunk-errors*))
	(unless tmp
	  (format *log-output* "No error measure found for alg ~a, rep ~a, fold ~a~%" alg2 rep fold)
	  )
	(push tmp errs2)
	))
    (when (> (length errs1) 1)
      (format *stats-output* "pvalPairedTTest ~a ~a: ~,5f~%" alg1 alg2 (ttest errs1 errs2)))
    )))

(defun mcnemar (rightwrong wrongright &optional (continuity t))
  (let (t1)
    (setq t1
	  (if (= 0 (+ rightwrong wrongright))
	      0.0
	      (progn
	        (setq t1 (abs (- rightwrong wrongright)))
		(when continuity
		  (unless (= rightwrong wrongright)
		    (setq t1 (- t1 1))))
		(setq t1 (* t1 t1))
		(setq t1 (/ t1 (+ rightwrong wrongright)))
		(chisq-cdf t1 1))))
    (- 1.0 t1)
    ))



(defun ttest (errs1 errs2)
  (block nil
    (format *log-output* "~a/~a~%" errs1 errs2)
    (progn
      (unless (listp errs1) (return "?"))
      (unless (listp errs2) (return "?"))
      (unless errs1 (return "?"))
      (unless errs2 (return "?"))
      (let* ((n (length errs1))
	     (diffs (- errs1 errs2))
	     (m1 (mean diffs))
	     (s1 (standard-deviation diffs))
	     (t1 (format *log-output* "~a/~a/~a~a~%" n m1 s1 (= 0.0 s1)))
	     pval se tval)
	(if (= 0.0 s1)
	    (progn
	      (format *log-output* "!!!!!!!!!!!!!")
	      "?")
	    (progn
	      (setq se (/ s1 (sqrt n)))
	      (setq tval (/ m1 se))
	      (* 2.0 (- 1.0 (t-cdf (abs tval) (- n 1))))))
	))))

;;; !!!
(defun class-stats-chunk (&optional rep fold)
  "Calculate and show all classification statistics for all the
   algorithms for which we do have data"
  (dolist (alg (get-algorithms *chunk-predictions*))
    (class-stats-chunk-alg alg rep fold)
      ))
  
;;; !!!
(defun class-stats-all ()
  "Calculate and show all classification statistics for all the
   algorithms for which we do have data"
  (let ((algs  (get-algorithms *all-predictions*))
	algs2)
  (dolist (alg algs)
    (class-stats-all-alg alg))
  ))

;;; !!!
(defun class-stats-pairs ()
  "Calculate and show all classification statistics for 
   algorithm pairings for which we do have data"
  (let ((algs  (get-algorithms *all-predictions*))
	algs2)
  (setq algs2 algs)
  (dolist (alg1 algs)
    (setq algs2 (remove alg1 algs2))
    (dolist (alg2 algs2)
      (class-stats-pair alg1 alg2)
      ))
  ))
  
(defun class-calc-stats (diffs targets alg rep fold)
  "Calculate regression statistics. Alg rep and fold are
   given for printing only. if fold and/or rep is nil,
   that output is suppressed"
  (let (se n sum (tmp nil) myn stdev)
    (setq n (length diffs))
    (setq sum (reduce #'+ diffs))
    (setq err (/ sum n))
    (setq text (format nil " ~a~a~a"
		       alg
		       (if rep (format nil " ~d" rep) "")
		       (if fold (format nil " ~d" fold) "")))
    (format *stats-output* "Error~a: ~f~%" text err)
    ;; if we have a repetition or fold number ....
    (if (or rep fold)
	;; store that value in the hash for use for the t-test
	(setf (gethash (list alg rep fold) *chunk-errors*) err)
	;; else (this is the call for the summary or an individual batch)
	;; calculate the standard error of the mean, if possible
	;; repetitions and folds are treated equally here, though
	;; repetitions depend on more correleated data
	(progn
          (when (> *nr-fold* 1)
	    (dotimes (myfold *nr-fold*)
	      (dotimes (myrep *nr-rep*)
	        (push (gethash (list alg myrep myfold) *chunk-errors*) tmp)
	        ))
	    (setq myn (length tmp))
	    (setq stdev (standard-deviation tmp))
	    (setq tmp (/ stdev (sqrt myn)))
	    (format *stats-output* "StdDevOfError ~a: ~f~%" alg stdev)
	    (format *stats-output* "StdErrOfError ~a: ~f~%" alg tmp)
	    )))
    ))

;;; ================== 

(defun as-string (what)
  "convert an atom name into a lower case string"
  (if (stringp what)
      what
      (if (atom what)
	  (string-downcase (symbol-name what))
	  (error "Need either string or symbols for as-string: ~a~%" what))))

(defun as-hashkey (what)
  "we use strings for the hash, but this can be changed by
   changing this function"
  (as-string what))


(defun sqr (x)
  (* x x))

(defun test-class ()
  (read-chunk-targets "allrep_1_0.targets")
  (read-chunk-predictions "allrep_1_0_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_0_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_0_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_0_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 0)

  (read-chunk-targets "allrep_1_1.targets")
  (read-chunk-predictions "allrep_1_1_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_1_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_1_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_1_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 1)

  (read-chunk-targets "allrep_1_2.targets")
  (read-chunk-predictions "allrep_1_2_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_2_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_2_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_2_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 2)

  (read-chunk-targets "allrep_1_3.targets")
  (read-chunk-predictions "allrep_1_3_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_3_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_3_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_3_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 3)

  (read-chunk-targets "allrep_1_4.targets")
  (read-chunk-predictions "allrep_1_4_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_4_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_4_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_4_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 4)

  (read-chunk-targets "allrep_1_5.targets")
  (read-chunk-predictions "allrep_1_5_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_5_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_5_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_5_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 5)

  (read-chunk-targets "allrep_1_6.targets")
  (read-chunk-predictions "allrep_1_6_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_6_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_6_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_6_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 6)

  (read-chunk-targets "allrep_1_7.targets")
  (read-chunk-predictions "allrep_1_7_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_7_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_7_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_7_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 7)

  (read-chunk-targets "allrep_1_8.targets")
  (read-chunk-predictions "allrep_1_8_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_8_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_8_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_8_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 8)

  (read-chunk-targets "allrep_1_9.targets")
  (read-chunk-predictions "allrep_1_9_c50boost.pred" 'c50boost)
  (read-chunk-predictions "allrep_1_9_c50tree.pred" 'c50tree)
  (read-chunk-predictions "allrep_1_9_clemMLP.pred" "clemMLP")
  (read-chunk-predictions "allrep_1_9_lindiscr.pred" 'lindiscr)
  (class-stats-chunk 0 9)

  (class-stats-all)
  (class-stats-pairs)

  )

(defun test-regr ()
  (read-chunk-targets "abalone_1_0.targets")
  (read-chunk-predictions "abalone_1_0_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 0)
  
  (read-chunk-targets "abalone_1_1.targets")
  (read-chunk-predictions "abalone_1_1_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 1)
  
  (read-chunk-targets "abalone_1_2.targets")
  (read-chunk-predictions "abalone_1_2_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 2)
  
  (read-chunk-targets "abalone_1_3.targets")
  (read-chunk-predictions "abalone_1_3_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 3)
  
  (read-chunk-targets "abalone_1_4.targets")
  (read-chunk-predictions "abalone_1_4_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 4)
  
  (read-chunk-targets "abalone_1_5.targets")
  (read-chunk-predictions "abalone_1_5_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 5)
  
  (read-chunk-targets "abalone_1_6.targets")
  (read-chunk-predictions "abalone_1_6_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 6)
  
  (read-chunk-targets "abalone_1_7.targets")
  (read-chunk-predictions "abalone_1_7_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 7)
  
  (read-chunk-targets "abalone_1_8.targets")
  (read-chunk-predictions "abalone_1_8_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 8)
  
  (read-chunk-targets "abalone_1_9.targets")
  (read-chunk-predictions "abalone_1_9_cubistdemo.pred" 'cubistdemo)
  (regr-stats-chunk 0 9)
  
  (regr-stats-all)
)


