; ModuleID = 'main'
source_filename = "main"

define i32 @foo() {
entry:
  %z = alloca i32, align 4
  %y = alloca i32, align 4
  %x = alloca i32, align 4
  br label %loop_start

loop_start:                                       ; preds = %loop_end3, %entry
  br i1 true, label %loop_body, label %loop_end

loop_body:                                        ; preds = %loop_start
  br label %loop_start1

loop_end:                                         ; preds = %loop_start
  ret i32 1

loop_start1:                                      ; preds = %else, %loop_body
  br i1 true, label %loop_body2, label %loop_end3

loop_body2:                                       ; preds = %loop_start1
  br i1 true, label %then, label %else

loop_end3:                                        ; preds = %loop_start1
  br label %loop_start

then:                                             ; preds = %loop_body2
  %0 = call i32 @foo()
  ret i32 %0

else:                                             ; preds = %loop_body2
  br label %loop_start1
}

define i32 @main(i32 %x) {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  %x2 = load i32, i32* %x1, align 4
  ret i32 %x2
}
