;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((nil . (
	 (eval . (let
		     ((root
		       (projectile-project-root)))
		   (setq-local company-clang-arguments
			       (list
				(concat "-I" root "include")))
		   (setq-local flycheck-clang-include-path
			       (list
				(concat root "include")))))))
 (c++-mode . ((c-basic-offset . 2)))
 (c-mode . ((c-basic-offset . 2))))
