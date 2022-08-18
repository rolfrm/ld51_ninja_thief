;;to profile: ./run foxday2.lisp | sort | uniq -c |sort -n

(define window-title "funky")
(load "models.lisp")


(defvar +dt+ 3.12)
(defvar +dt2+ 0.12)
(defvar zoom 0.08)
(defvar time2 0.0)
(defvar game-update
  (let ((mx nil) (my nil))
    (lambda (events)
      ;(println mz)
      (incf time2 (+ 0.0005 (* (+ 1.0 (sin time2)) 0.0025)))
      (push-event (list 'frame time2))
      (for-each x events
                (when (eq (car x) 'mouse-leave)
                  (set! mx nil)
                  (set! my nil))
                (when (eq (car x) 'mouse-scroll)
                  (let* ((amount (caddr x))
                         (amount2 (math:pow 1.2 amount)))
                    (set! zoom (* zoom amount2))
                    (println zoom)))
                (when (eq (car x) 'mouse-move-delta)
                  (incf +dt2+ (* -0.01 (cadr x)))
                  (incf +dt+ (* 0.01 (caddr x)))
                  )
                (when (eq (car x) 'mouse-move)
                  (let ((mx2 (rational (cadr x)))
                        (my2 (rational (cddr x))))
                    (when (and mx my)
                      (push-event (list 'mouse-move-delta (- mx2 mx) (- my2 my))))
                   
                    (set! mx mx2)
                    (set! my my2)
                 
                    ))
      ))))

(defvar dots nil)
(dotimes! i 100
          (push! dots (* (- i 50) 0.3)))

(define start-time (foxgl:timestamp))

(defun yfunc(x2)
  (let ((x (* 0.1 (+ (* time2 40.0) x2))))
    (* 10.0 (cos (* x 3.3)) (sin x))))

(define model
    '(view :perspective (1.0 1.0 0.01 100.0)
      (scale (40.0 40.0)
        (translate (-0.5 -0.5 -2)
         (rgb (0 0 0)
            (bind square-model))))
      (depth
       (translate (-0.0 -0.0 -2)
        (scale (bind zoom)
         (rotate ((bind +dt+) (bind +dt2+) 0.0)
          (translate (0 0 -0.1)
           (rgb (0.8 0.8 0.8)
            (translate (-50)
             (scale (100.0 0.1)
              (bind square-model)))
            (translate (0 -50)
             (scale (0.1 100.0)
              (bind square-model)))
            ))
          (rgb (1.0 1.0 0.0)
          (line
           (for i (bind dots)
            (point (bind i) (bind (yfunc i)))
            ))
           )
          (translate (0 0 0.5)
          (rgb (1.0 0.0 0.0)
          (line
           (for i (bind dots)
            (point (bind i) (bind (yfunc (+ 100.0 i))))
            ))
           ))
          (translate (0 0 1)
          (rgb (0.0 0.0 1.0)
          (line
           (for i (bind dots)
            (point (bind i) (bind (* 0.5 (+ (yfunc i) (yfunc (+ 100.0  i)) ))))
            ))
           ))
          (line
           (point -10.0 -9.0)

           (point -10.0 9.0)
           (point -9.0 10.0)

           (point 9.0 10.0)
           (point 10.0 9.0)

           (point 10.0 -9.0)
           (point 9.0 -10.0)

           (point -9.0 -10.0)
           (point -10.0 -9.0)

           )
          (translate (0 (bind (yfunc 0.0)))
          (translate (0.5 -1)

           (scale 0.05
            (text (bind (math:round (yfunc 0.0) 2))))
          )))))
        )))
