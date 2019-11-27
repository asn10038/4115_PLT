; ModuleID = 'example2.bc'
source_filename = "example2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local i32 @sum(i32 %a, i32 %e) local_unnamed_addr #0 {
entry:
  %cmp6 = icmp slt i32 %a, %e
  br i1 %cmp6, label %while.body, label %while.end

while.body:                                       ; preds = %entry, %while.body
  %a.addr.07 = phi i32 [ %mul, %while.body ], [ %a, %entry ]
  %add = shl i32 %a.addr.07, 1
  %mul = add i32 %add, 2
  %cmp = icmp slt i32 %mul, %e
  br i1 %cmp, label %while.body, label %while.end

while.end:                                        ; preds = %while.body, %entry
  %a.addr.0.lcssa = phi i32 [ %a, %entry ], [ %mul, %while.body ]
  ret i32 %a.addr.0.lcssa
}

attributes #0 = { norecurse nounwind readnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0 (https://github.com/llvm/llvm-project.git c2ca003baffbdb58b4a2e5c9c5163122844a82d3)"}
