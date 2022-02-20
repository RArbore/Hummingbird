;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((nil . (
	 (eval . (let
		     ((root
		       (projectile-project-root)))
		   (setq-local flycheck-clang-args
			       (list
				(concat "-I" root "include") (concat "-std=c++20")))
		   (setq-local flycheck-clang-include-path
			       (list
				(concat root "include")))))))
 (c++-mode . ((c-basic-offset . 2)))
 (c-mode . ((mode . c++))))
