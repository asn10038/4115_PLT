; ModuleID = 'example.bc'
source_filename = "example1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@g = common dso_local local_unnamed_addr global i32 0, align 4
@h = common dso_local local_unnamed_addr global i32 0, align 4

; Function Attrs: norecurse nounwind readonly uwtable
define dso_local i32 @test(i32 %condition) local_unnamed_addr #0 {
entry:
  %cmp = icmp eq i32 %condition, 1
  %g.val = load i32, i32* @g, align 4
  %h.val = load i32, i32* @h, align 4
  %x.0 = select i1 %cmp, i32 %g.val, i32 %h.val
  ret i32 %x.0
}

attributes #0 = { norecurse nounwind readonly uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0 (https://github.com/llvm/llvm-project.git c2ca003baffbdb58b4a2e5c9c5163122844a82d3)"}