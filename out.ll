; ModuleID = 'main'
source_filename = "main"

%a = type { i32, i32 }

@a_type_anchor = external global %a

define i32 @foo() {
entry:
  %c = alloca i32, align 4
  %x = alloca i32, align 4
  %b = alloca i32*, align 8
  %a = alloca [2 x [3 x i32]], align 4
  %0 = getelementptr inbounds [2 x [3 x i32]], [2 x [3 x i32]]* %a, i32 0, i32 0
  %1 = getelementptr inbounds [3 x i32], [3 x i32]* %0, i32 0, i32 0
  store i32 0, i32* %1, align 4
  %2 = getelementptr inbounds i32, i32* %1, i32 1
  store i32 1, i32* %2, align 4
  %3 = getelementptr inbounds i32, i32* %2, i32 1
  store i32 2, i32* %3, align 4
  %4 = getelementptr inbounds i32, i32* %3, i32 1
  store i32 0, i32* %4, align 4
  %5 = getelementptr inbounds i32, i32* %4, i32 1
  store i32 0, i32* %5, align 4
  %6 = getelementptr inbounds i32, i32* %5, i32 1
  store i32 0, i32* %6, align 4
  %7 = getelementptr inbounds i32, i32* %6, i32 1
  br label %loop_start

loop_start:                                       ; preds = %loop_body, %entry
  %8 = getelementptr inbounds [2 x [3 x i32]], [2 x [3 x i32]]* %a, i32 0, i32 1
  %9 = getelementptr inbounds [3 x i32], [3 x i32]* %8, i32 0, i32 2
  %10 = load i32, i32* %9, align 4
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %loop_body, label %loop_end

loop_body:                                        ; preds = %loop_start
  store i32 2, i32* %x, align 4
  br label %loop_start

loop_end:                                         ; preds = %loop_start
  %b1 = load i32*, i32** %b, align 8
  %12 = load i32, i32* %b1, align 4
  %13 = add i32 %12, 1
  store i32 %13, i32* %c, align 4
  %14 = getelementptr inbounds [2 x [3 x i32]], [2 x [3 x i32]]* %a, i32 0, i32 0
  %15 = getelementptr inbounds [3 x i32], [3 x i32]* %14, i32 0, i32 0
  %16 = load i32, i32* %15, align 4
  ret i32 %16
}

define i32 @main(i32 %x) {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  ret i32 0
}
