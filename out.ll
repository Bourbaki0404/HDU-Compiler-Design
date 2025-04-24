; ModuleID = 'main'
source_filename = "main"

define i32 @foo(i32 %a, i32 %a1) {
entry:
  %a2 = alloca i32, align 4
  store i32 %a, i32* %a2, align 4
  %a13 = alloca i32, align 4
  store i32 %a1, i32* %a13, align 4
  %a4 = load i32, i32* %a2, align 4
  %a5 = load i32, i32* %a2, align 4
  %0 = mul i32 4, %a5
  %a6 = load i32, i32* %a2, align 4
  %a7 = load i32, i32* %a2, align 4
  %1 = mul i32 4, %a7
  %2 = add i32 1, %1
  ret i32 %2
}

define i32 @main() {
entry:
  ret i32 2
}
