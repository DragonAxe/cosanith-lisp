target triple = "x86_64-pc-linux-gnu"

; Definition of main function
define i32 @main() {   ; i32()*
  %a = add i32 3, 8
  %b = add i32 3, %a

  ret i32 %b
}
