; ModuleID = './test_input.cpp'
source_filename = "./test_input.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local noundef i32 @_Z3fooPA3_A4_i([3 x [4 x i32]]* noundef %0) #0 {
  %2 = alloca [3 x [4 x i32]]*, align 8
  %3 = alloca [3 x [4 x i32]], align 16
  store [3 x [4 x i32]]* %0, [3 x [4 x i32]]** %2, align 8
  %4 = load [3 x [4 x i32]]*, [3 x [4 x i32]]** %2, align 8
  %5 = getelementptr inbounds [3 x [4 x i32]], [3 x [4 x i32]]* %4, i64 1
  %6 = getelementptr inbounds [3 x [4 x i32]], [3 x [4 x i32]]* %5, i64 0, i64 2
  %7 = getelementptr inbounds [4 x i32], [4 x i32]* %6, i64 0, i64 3
  %8 = load i32, i32* %7, align 4
  %9 = add nsw i32 2, %8
  ret i32 %9
}

; Function Attrs: mustprogress noinline norecurse nounwind optnone uwtable
define dso_local noundef i32 @main(i32 noundef %0) #1 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, i32* %2, align 4
  store i32 %0, i32* %3, align 4
  %4 = load i32, i32* %3, align 4
  ret i32 %4
}

attributes #0 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress noinline norecurse nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
