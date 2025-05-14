; ModuleID = 'main'
source_filename = "main"

define i32 @div(i32 %a, i32 %b) {
entry:
  %a1 = alloca i32, align 4
  store i32 %a, i32* %a1, align 4
  %b2 = alloca i32, align 4
  store i32 %b, i32* %b2, align 4
  %a3 = load i32, i32* %a1, align 4
  %b4 = load i32, i32* %b2, align 4
  %0 = sdiv i32 %a3, %b4
  ret i32 %0
}

define i32 @main() {
entry:
  ret i32 0
}
