#lang racket
(define xs '(1 2 3 4))
(define func (lambda (x) (+ x 1)))
;;map
(define (my-map func x)
  (if (null? x)
      null
      (cons (func (car x)) (my-map func (cdr x)))))

;;fileter
(define (my-filter func x)
  (if (null? x)
      null
      (if (func (car x))
          (cons (car x) (my-filter func (cdr x)))
          (my-filter func (cdr x)))))

;;reduce
(define (my-reduce func xs init)
  (if (null? xs)
      init
      (func (car xs) (my-reduce func (cdr xs) init))))