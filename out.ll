; ModuleID = 'main'
source_filename = "main"

define i32 @foo([3 x [4 x i32]]* %x) {
entry:
  %x1 = alloca [3 x [4 x i32]]*, align 8
  store [3 x [4 x i32]]* %x, [3 x [4 x i32]]** %x1, align 8
  %u = alloca [3 x [4 x i32]], align 4
  %x2 = load [3 x [4 x i32]]*, [3 x [4 x i32]]** %x1, align 8
  %0 = getelementptr [3 x [4 x i32]], [3 x [4 x i32]]* %x2, i32 1
  %1 = getelementptr inbounds [3 x [4 x i32]], [3 x [4 x i32]]* %0, i32 0, i32 2
  %2 = getelementptr inbounds [4 x i32], [4 x i32]* %1, i32 0, i32 3
  %3 = load i32, i32* %2, align 4
  %4 = add i32 2, %3
  ret i32 %4
}

define i32 @main(i32 %x) {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  %x2 = load i32, i32* %x1, align 4
  ret i32 %x2
}
