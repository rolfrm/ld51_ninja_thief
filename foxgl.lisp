;(println (foxgl:timestamp))

(defmacro measure(&rest body)
  `(let ((time-start (foxgl:timestamp))
         (result (progn ,@body))
         (time-end (foxgl:timestamp)))
                                        ;(println (list 'operation-took (/ (rational (- time-end time-start)) 1000000.0)))
    result))

(define measure2-inlet 0)
(defmacro measure2 (name &rest body)
  `(let ((time-start (foxgl:timestamp))
         (l (incf measure2-inlet 1))
    
         (result (progn ,@body))
         (time-end (foxgl:timestamp)))
                              
    (println (list l ,name (/ (rational (- time-end time-start)) 1000000.0)))
                                        ;  )
    (incf measure2-inlet -1)
    result))

;(defmacro measure2 (name &rest body)
;  `(progn ,@body))


(defmacro with-tracing(&rest body)
  `(let ((result nil))
    (lisp:trace t)
    (set! result
     (progn ,@body))
    (lisp:trace nil)
    result))

(defun plist-rest (lst func)
  (while lst
         (let ((fst (car lst)))
           (if (symbol? fst)
               (set! lst (cddr lst))
               (do
                (set! lst (cdr lst))
                (func fst))))))

(defun mat4-identity ()
  (let ((m (make-vector 16 (float32 0.0))))
    (vector-set! m 0 (float32 1.0))
    (vector-set! m 5 (float32 1.0))
    (vector-set! m 10 (float32 1.0))
    (vector-set! m 15 (float32 1.0))
    m
    ))

(defun mat4-translation (x y z)
  (let ((m (mat4-identity)))
    (vector-set! m 12 (float32 x))
    (vector-set! m 13 (float32 y))
    (vector-set! m 14 (float32 z))
    m))

(defun mat4-scale (x y z)
  (let ((m (mat4-identity)))
    (vector-set! m 0 (float32 x))
    (vector-set! m 5 (float32 y))
    (vector-set! m 10 (float32 z))
    m))
(defun mat3-identity ()
  (let ((m (make-vector 9 (float32 0.0))))
    (vector-set! m 0 (float32 1.0))
    (vector-set! m 4 (float32 1.0))
    (vector-set! m 8 (float32 1.0))
    m
    ))

(defun mat3-scale (x y)
  (let ((m (mat3-identity)))
    (vector-set! m 0 (float32 x))
    (vector-set! m 4 (float32 y))
    m))

(defun render-model (model)
  (foxgl:init)
  ;(println 'render-model)
  (foxgl:render-model2 model)
  )
(define get-framebuffer nil)
(define load-framebuffer nil)
(define foxgl:current-color '(1 1 1 1))
(define foxgl:current-transform (mat4-identity))
(define foxgl:framebuffer-cache (make-hashtable t nil))
(define foxgl:polygon-cache2 nil)
(define foxgl:polygon-cache (make-hashtable t nil))
(define foxgl:square-buffers nil)
(defun foxgl:render-sub-models (model)
  (plist-rest (cdr model) foxgl:render-model2))
(define foxgl:test-tex nil)

(defun foxgl:get-framebuffer (model)
  (hashtable-ref foxgl:framebuffer-cache model))
(defun foxgl:load-framebuffer (model size)
  (let ((bf (foxgl:create-framebuffer (car size) (cadr size))))
    (hashtable-set! foxgl:framebuffer-cache model bf)
    bf))
(defun foxgl:render-model2 (model)

  (let ((sym (car model))
        (render-sub t)
        )
    (case sym
      (color
       (let ((prev-color foxgl:current-color))
        
        (match rgb (plookup (cdr model) ':rgb)
               (set! foxgl:current-color rgb)
               )
        (match rgb (plookup (cdr model) ':rgba)
               (set! foxgl:current-color rgb)
               )
        (set! render-sub nil)
        (foxgl:render-sub-models model)
        (set! foxgl:current-color prev-color)
        ))
      (measure-model
       (progn
         (measure (foxgl:render-model2 (cadr model)))
         ;(println (cadr model))
         (set! render-sub nil)
         ))
      (ref 
       (foxgl:render-model2 (symbol-value (cadr model) t)))
      (view
       (let ((prev-transform foxgl:current-transform))
         (match p (plookup (cdr model) :perspective)
                (let ((fov (car p))
                      (aspect (cadr p))
                      (near (caddr p))
                      (far (cadddr p))
                      (prev-tform foxgl:current-transform))
                  (set! foxgl:current-transform (mat4:perspective fov aspect near far))
                  ))
         (match p (plookup (cdr model) :orthographic)
                (let ((w (car p))
                      (h (cadr p))
                      (z (caddr p))
                      (prev-tform foxgl:current-transform))
                  (set! foxgl:current-transform (mat4:orthographic w h z))
                  ))
         (set! render-sub nil)
         (foxgl:render-sub-models model)
         (set! foxgl:current-transform prev-transform)
         ))
      (transform
       (let ((prev-tform foxgl:current-transform)
             (new-transform (mat4-identity)))
         (when foxgl:current-transform
           (math:*! new-transform new-transform foxgl:current-transform))
         
         (match tlate (plookup (cdr model) ':translate)
                (math:translate! new-transform
                                 (car tlate) (cadr tlate) (or (caddr tlate) 0.0)))
         
         
         (match scale (plookup (cdr model) ':scale)
                (math:scale! new-transform 
                             (car scale) (cadr scale) (or (caddr scale) 1.0)))
         
         (match rotation (plookup (cdr model) ':rotate)
                
                (math:rotate!  new-transform  (car rotation)
                               (or (cadr rotation) 0.0)
                               (or (caddr rotation) 0.0)))
         (set! foxgl:current-transform new-transform)
         (set! render-sub nil)
         (foxgl:render-sub-models model)
         (set! foxgl:current-transform prev-tform)
         
         ))
      (render-callback
       ((cadr model) model)
       )
      (unit-square
       (progn
         (foxgl:color foxgl:current-color)
         (foxgl:transform (or foxgl:current-transform (mat4-identity)))
         (foxgl:square)
         ))
      (print-model
       (println 'print-model)
       (println (cdr model))

       )
      (blend
       (progn
         (foxgl:blend t)
         (set! render-sub nil)
         (foxgl:render-sub-models model)
         (foxgl:blend nil)
         ))
      (depth
       (progn
      
         (foxgl:depth t)
                                        ;(println (glerror))
         (set! render-sub nil)
         (foxgl:render-sub-models model)
         (foxgl:depth nil)
         ))
      (text
       (progn
         (foxgl:color foxgl:current-color)
         (foxgl:transform (or foxgl:current-transform (mat4-identity)))
         (foxgl:blit-text (cadr model)(or foxgl:current-transform (mat4-identity)))
         ))
      (flat
       (let ((fb (foxgl:get-framebuffer model))
             (prev-transform foxgl:current-transform)
             )
         (when (null? fb)
           (let ((s (plookup (cdr model) :size)))
             (set! fb (foxgl:load-framebuffer model (or s '(100 100))))))
         (foxgl:bind-framebuffer fb)
         (set! foxgl:transform (mat4-identity))
         (foxgl:unbind-framebuffer fb)
         (set! foxgl:current-transform prev-transform)
         (unless foxgl:square-buffers
           
           (set! foxgl:square-buffers
                 (list
                  (foxgl:load-polygon (list-to-array '(0 0 1 0 0 1 1 1)))
                  (foxgl:load-polygon (list-to-array '(0 0 1 0 0 1 1 1)))))
           
           )
         (foxgl:bind-texture (foxgl:framebuffer-texture fb))
         (foxgl:color foxgl:current-color)
         (foxgl:transform foxgl:current-transform)
         (foxgl:blend t)
         (foxgl:blit-polygon foxgl:square-buffers)
         (foxgl:blend nil)
         (foxgl:bind-texture nil)
         (set! render-sub nil)
         ))
      (polygon
       (let ((dims 2)
             (poly (plookup (cdr model) :2d-triangle-strip)))
         (unless poly
           (set! poly (plookup (cdr model) :3d-triangle-strip))
           (set! dims 3)
           )
         (when poly
           (let ((r (hashtable-ref foxgl:polygon-cache (cdr model))))
             (unless r
               (println (list 'new-poly r))
               (set! r (cons 'poly (foxgl:load-polygon (list-to-array poly) dims)))
               (hashtable-set foxgl:polygon-cache (cdr model) r)
               (push! foxgl:polygon-cache2 (cdr model))
               (push! foxgl:polygon-cache2 r)
                                        ;(register-finalizer r cache-delete)
               )
             (foxgl:color foxgl:current-color)
             (foxgl:transform foxgl:current-transform)
             (foxgl:blit-polygon (cdr r))
             )
           )))
      (hidden
       (set! render-sub nil)))
    (when render-sub
      (plist-rest (cdr model) foxgl:render-model2))  
    ))

(let ((m1 (mat4-translation 4 0 0))
      (m2 (mat4-translation 3 2 1)))
  (mat4:print m1)
  (println "")
  (mat4:print m2)
  (println "")

  (mat4:print (math:* m2 m1))
  (println "")
  (println "")
  )

(println (math:* (mat3-scale 2 3) (mat3-scale 4 5)))
(println lisp:*web-environment*)
(unless (or t lisp:*web-environment*)
  (thread:join (thread:start (lambda () (println 'thread!))))
  (thread:join (thread:start (lambda () (println 'thread!))))
  
  (let ((srv (tcp:listen 8893))
        (cli (tcp:connect "127.0.0.1" 8893)))
    (let ((cli2 (tcp:accept srv))
        (v (make-vector 4 1))
          (v2 (make-vector 10 (byte 0)))
        )
      (vector-set! v 0 10101010101010)
      (fd:write cli2 v)
      (println (list 'read (fd:read cli v2)))
      (println (list srv cli cli2 v2))
      
      (fd:close cli2)
      (fd:close cli)
      (fd:close srv)
      ))
  )

(defvar audio:note-low (* 12.0 8.0))
(defun audio:note-to-frequency(note)
  (* 440.0 (math:pow 2.0 (/ (rational note) 12.0)))
  )

(defun process-song(song buffer sample-rate phase speed)
  (let ((i 0)
        (sample (rational sample-rate))
        (llen (vector-length buffer))
        (phase-incr (/ 1.0 phase)))
    
    (defun rec(song)
      
      (let ((fst (car song))
            (result 0.0))
        (when (eq fst 'melody)
          (let ((lst (cdr song))
                (p phase))
            (loop lst
                  (if (< p 0.25)
                      (progn ;; found note
                        (set! result (sin (* 6.28 (* p (audio:note-to-frequency (car lst))))))          (set! lst nil)
                        
                        )
                      (progn
                        (set! lst (cdr lst))
                        (set! p (- p 0.25)))))
            )
          )
        result
        ))
    (loop (< i llen)
          (vector-set! buffer i (float32 (rec song)))
          (incf phase phase-incr)
          (incf i 1))
    ))

(define foxgl:key-up 264)
(define foxgl:key-w 87)
(define foxgl:key-a 65)
(define foxgl:key-s 83)
(define foxgl:key-d 68)



;(render-model2 nil)
;(lisp:exit 0)
