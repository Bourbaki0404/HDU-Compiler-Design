; ModuleID = './test_input.cpp'
source_filename = "./test_input.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const._Z3divii.c = private unnamed_addr constant [3 x i32] [i32 1, i32 2, i32 3], align 4

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local noundef i32 @_Z3divii(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca [3 x i32], align 4
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  %7 = bitcast [3 x i32]* %6 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %7, i8* align 4 bitcast ([3 x i32]* @__const._Z3divii.c to i8*), i64 12, i1 false)
  br label %8

8:                                                ; preds = %2
  %9 = getelementptr inbounds [3 x i32], [3 x i32]* %6, i64 0, i64 1
  %10 = load i32, i32* %9, align 4
  %11 = icmp sgt i32 %10, 1
  br i1 %11, label %12, label %18

12:                                               ; preds = %8
  %13 = load i32, i32* %4, align 4
  %14 = add nsw i32 2, %13
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [3 x i32], [3 x i32]* %6, i64 0, i64 %15
  %17 = load i32, i32* %16, align 4
  store i32 %17, i32* %3, align 4
  br label %23

18:                                               ; preds = %8
  %19 = getelementptr inbounds [3 x i32], [3 x i32]* %6, i64 0, i64 1
  %20 = load i32, i32* %19, align 4
  %21 = add nsw i32 1, %20
  %22 = sub nsw i32 %21, 2
  store i32 %22, i32* %3, align 4
  br label %23

23:                                               ; preds = %18, %12
  %24 = load i32, i32* %3, align 4
  ret i32 %24
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: mustprogress noinline norecurse nounwind optnone uwtable
define dso_local noundef i32 @main() #2 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  ret i32 0
}

attributes #0 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly nofree nounwind willreturn }
attributes #2 = { mustprogress noinline norecurse nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
