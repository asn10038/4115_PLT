; ModuleID = 'bubble.bc'
source_filename = "bubble.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@__const.main.arr = private unnamed_addr constant [7 x i32] [i32 64, i32 34, i32 25, i32 12, i32 22, i32 11, i32 90], align 16
@str = private unnamed_addr constant [15 x i8] c"Sorted array: \00", align 1

; Function Attrs: nofree norecurse nounwind uwtable
define dso_local void @swap(i32* nocapture %xp, i32* nocapture %yp) local_unnamed_addr #0 {
entry:
  %0 = load i32, i32* %xp, align 4, !tbaa !2
  %1 = load i32, i32* %yp, align 4, !tbaa !2
  store i32 %1, i32* %xp, align 4, !tbaa !2
  store i32 %0, i32* %yp, align 4, !tbaa !2
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: nofree norecurse nounwind uwtable
define dso_local void @bubbleSort(i32* nocapture %arr, i32 %n) local_unnamed_addr #0 {
entry:
  %sub = add i32 %n, -1
  %cmp32 = icmp sgt i32 %sub, 0
  br i1 %cmp32, label %for.cond1.preheader, label %for.end16

for.cond1.preheader:                              ; preds = %entry, %for.inc14
  %i.033 = phi i32 [ %inc15, %for.inc14 ], [ 0, %entry ]
  %0 = sub i32 %sub, %i.033
  %1 = zext i32 %0 to i64
  %2 = xor i32 %i.033, -1
  %sub3 = add i32 %2, %n
  %cmp430 = icmp sgt i32 %sub3, 0
  br i1 %cmp430, label %for.body5.preheader, label %for.inc14

for.body5.preheader:                              ; preds = %for.cond1.preheader
  %.pre = load i32, i32* %arr, align 4, !tbaa !2
  %xtraiter = and i64 %1, 1
  %3 = icmp eq i32 %0, 1
  br i1 %3, label %for.inc14.loopexit.unr-lcssa, label %for.body5.preheader.new

for.body5.preheader.new:                          ; preds = %for.body5.preheader
  %unroll_iter = sub nsw i64 %1, %xtraiter
  br label %for.body5

for.body5:                                        ; preds = %for.inc.1, %for.body5.preheader.new
  %4 = phi i32 [ %.pre, %for.body5.preheader.new ], [ %9, %for.inc.1 ]
  %indvars.iv = phi i64 [ 0, %for.body5.preheader.new ], [ %indvars.iv.next.1, %for.inc.1 ]
  %niter = phi i64 [ %unroll_iter, %for.body5.preheader.new ], [ %niter.nsub.1, %for.inc.1 ]
  %indvars.iv.next = or i64 %indvars.iv, 1
  %arrayidx7 = getelementptr inbounds i32, i32* %arr, i64 %indvars.iv.next
  %5 = load i32, i32* %arrayidx7, align 4, !tbaa !2
  %cmp8 = icmp sgt i32 %4, %5
  br i1 %cmp8, label %if.then, label %for.inc

if.then:                                          ; preds = %for.body5
  %arrayidx = getelementptr inbounds i32, i32* %arr, i64 %indvars.iv
  store i32 %5, i32* %arrayidx, align 4, !tbaa !2
  store i32 %4, i32* %arrayidx7, align 4, !tbaa !2
  br label %for.inc

for.inc:                                          ; preds = %for.body5, %if.then
  %6 = phi i32 [ %5, %for.body5 ], [ %4, %if.then ]
  %indvars.iv.next.1 = add nuw nsw i64 %indvars.iv, 2
  %arrayidx7.1 = getelementptr inbounds i32, i32* %arr, i64 %indvars.iv.next.1
  %7 = load i32, i32* %arrayidx7.1, align 4, !tbaa !2
  %cmp8.1 = icmp sgt i32 %6, %7
  br i1 %cmp8.1, label %if.then.1, label %for.inc.1

for.inc14.loopexit.unr-lcssa:                     ; preds = %for.inc.1, %for.body5.preheader
  %.unr = phi i32 [ %.pre, %for.body5.preheader ], [ %9, %for.inc.1 ]
  %indvars.iv.unr = phi i64 [ 0, %for.body5.preheader ], [ %indvars.iv.next.1, %for.inc.1 ]
  %lcmp.mod = icmp eq i64 %xtraiter, 0
  br i1 %lcmp.mod, label %for.inc14, label %for.body5.epil

for.body5.epil:                                   ; preds = %for.inc14.loopexit.unr-lcssa
  %indvars.iv.next.epil = add nuw nsw i64 %indvars.iv.unr, 1
  %arrayidx7.epil = getelementptr inbounds i32, i32* %arr, i64 %indvars.iv.next.epil
  %8 = load i32, i32* %arrayidx7.epil, align 4, !tbaa !2
  %cmp8.epil = icmp sgt i32 %.unr, %8
  br i1 %cmp8.epil, label %if.then.epil, label %for.inc14

if.then.epil:                                     ; preds = %for.body5.epil
  %arrayidx.epil = getelementptr inbounds i32, i32* %arr, i64 %indvars.iv.unr
  store i32 %8, i32* %arrayidx.epil, align 4, !tbaa !2
  store i32 %.unr, i32* %arrayidx7.epil, align 4, !tbaa !2
  br label %for.inc14

for.inc14:                                        ; preds = %for.inc14.loopexit.unr-lcssa, %for.body5.epil, %if.then.epil, %for.cond1.preheader
  %inc15 = add nuw nsw i32 %i.033, 1
  %exitcond36 = icmp eq i32 %inc15, %sub
  br i1 %exitcond36, label %for.end16, label %for.cond1.preheader

for.end16:                                        ; preds = %for.inc14, %entry
  ret void

if.then.1:                                        ; preds = %for.inc
  %arrayidx.1 = getelementptr inbounds i32, i32* %arr, i64 %indvars.iv.next
  store i32 %7, i32* %arrayidx.1, align 4, !tbaa !2
  store i32 %6, i32* %arrayidx7.1, align 4, !tbaa !2
  br label %for.inc.1

for.inc.1:                                        ; preds = %if.then.1, %for.inc
  %9 = phi i32 [ %7, %for.inc ], [ %6, %if.then.1 ]
  %niter.nsub.1 = add i64 %niter, -2
  %niter.ncmp.1 = icmp eq i64 %niter.nsub.1, 0
  br i1 %niter.ncmp.1, label %for.inc14.loopexit.unr-lcssa, label %for.body5
}

; Function Attrs: nofree nounwind uwtable
define dso_local void @printArray(i32* nocapture readonly %arr, i32 %size) local_unnamed_addr #2 {
entry:
  %cmp5 = icmp sgt i32 %size, 0
  br i1 %cmp5, label %for.body.preheader, label %for.end

for.body.preheader:                               ; preds = %entry
  %wide.trip.count = zext i32 %size to i64
  br label %for.body

for.body:                                         ; preds = %for.body, %for.body.preheader
  %indvars.iv = phi i64 [ 0, %for.body.preheader ], [ %indvars.iv.next, %for.body ]
  %arrayidx = getelementptr inbounds i32, i32* %arr, i64 %indvars.iv
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !2
  %call = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %0)
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body, %entry
  %putchar = tail call i32 @putchar(i32 10)
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local i32 @main() local_unnamed_addr #4 {
for.body5.preheader.i:
  %arr = alloca [7 x i32], align 16
  %0 = bitcast [7 x i32]* %arr to i8*
  call void @llvm.lifetime.start.p0i8(i64 28, i8* nonnull %0) #6
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull align 16 dereferenceable(28) %0, i8* nonnull align 16 dereferenceable(28) bitcast ([7 x i32]* @__const.main.arr to i8*), i64 28, i1 false)
  %arraydecay = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 0
  %arrayidx7.i = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  %1 = load i32, i32* %arrayidx7.i, align 4, !tbaa !2
  %cmp8.i = icmp slt i32 %1, 64
  br i1 %cmp8.i, label %if.then.i, label %for.inc.i

if.then.i:                                        ; preds = %for.body5.preheader.i
  %arrayidx.i = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 0
  store i32 %1, i32* %arrayidx.i, align 16, !tbaa !2
  store i32 64, i32* %arrayidx7.i, align 4, !tbaa !2
  br label %for.inc.i

for.inc.i:                                        ; preds = %if.then.i, %for.body5.preheader.i
  %2 = phi i32 [ %1, %for.body5.preheader.i ], [ 64, %if.then.i ]
  %arrayidx7.i.18 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  %3 = load i32, i32* %arrayidx7.i.18, align 8, !tbaa !2
  %cmp8.i.19 = icmp sgt i32 %2, %3
  br i1 %cmp8.i.19, label %if.then.i.112, label %for.inc.i.113

if.then.i.1:                                      ; preds = %for.inc.i.5
  %arrayidx.i.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 0
  store i32 %43, i32* %arrayidx.i.1, align 16, !tbaa !2
  store i32 %.pre.i.1, i32* %arrayidx7.i.1, align 4, !tbaa !2
  br label %for.inc.i.1

for.inc.i.1:                                      ; preds = %if.then.i.1, %for.inc.i.5
  %4 = phi i32 [ %43, %for.inc.i.5 ], [ %.pre.i.1, %if.then.i.1 ]
  %arrayidx7.i.1.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  %5 = load i32, i32* %arrayidx7.i.1.1, align 8, !tbaa !2
  %cmp8.i.1.1 = icmp sgt i32 %4, %5
  br i1 %cmp8.i.1.1, label %if.then.i.1.1, label %for.inc.i.1.1

if.then.i.2:                                      ; preds = %for.inc.i.1.4
  %arrayidx.i.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 0
  store i32 %34, i32* %arrayidx.i.2, align 16, !tbaa !2
  store i32 %.pre.i.2, i32* %arrayidx7.i.2, align 4, !tbaa !2
  br label %for.inc.i.2

for.inc.i.2:                                      ; preds = %if.then.i.2, %for.inc.i.1.4
  %6 = phi i32 [ %34, %for.inc.i.1.4 ], [ %.pre.i.2, %if.then.i.2 ]
  %arrayidx7.i.2.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  %7 = load i32, i32* %arrayidx7.i.2.1, align 8, !tbaa !2
  %cmp8.i.2.1 = icmp sgt i32 %6, %7
  br i1 %cmp8.i.2.1, label %if.then.i.2.1, label %for.inc.i.2.1

if.then.i.3:                                      ; preds = %for.inc.i.2.3
  %arrayidx.i.3 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 0
  store i32 %27, i32* %arrayidx.i.3, align 16, !tbaa !2
  store i32 %.pre.i.3, i32* %arrayidx7.i.3, align 4, !tbaa !2
  br label %for.inc.i.3

for.inc.i.3:                                      ; preds = %if.then.i.3, %for.inc.i.2.3
  %8 = phi i32 [ %27, %for.inc.i.2.3 ], [ %.pre.i.3, %if.then.i.3 ]
  %arrayidx7.i.3.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  %9 = load i32, i32* %arrayidx7.i.3.1, align 8, !tbaa !2
  %cmp8.i.3.1 = icmp sgt i32 %8, %9
  br i1 %cmp8.i.3.1, label %if.then.i.3.1, label %for.inc.i.3.1

if.then.i.4:                                      ; preds = %for.inc.i.3.2
  %arrayidx.i.4 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 0
  store i32 %22, i32* %arrayidx.i.4, align 16, !tbaa !2
  store i32 %.pre.i.4, i32* %arrayidx7.i.4, align 4, !tbaa !2
  br label %for.inc.i.4

for.inc.i.4:                                      ; preds = %if.then.i.4, %for.inc.i.3.2
  %10 = phi i32 [ %22, %for.inc.i.3.2 ], [ %.pre.i.4, %if.then.i.4 ]
  %arrayidx7.i.4.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  %11 = load i32, i32* %arrayidx7.i.4.1, align 8, !tbaa !2
  %cmp8.i.4.1 = icmp sgt i32 %10, %11
  br i1 %cmp8.i.4.1, label %if.then.i.4.1, label %for.inc.i.4.1

if.then.i.5:                                      ; preds = %for.inc.i.4.1
  %arrayidx.i.5 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 0
  store i32 %19, i32* %arrayidx.i.5, align 16, !tbaa !2
  store i32 %.pre.i.5, i32* %arrayidx7.i.5, align 4, !tbaa !2
  br label %for.inc14.i.5

for.inc14.i.5:                                    ; preds = %if.then.i.5, %for.inc.i.4.1
  %puts = tail call i32 @puts(i8* nonnull dereferenceable(1) getelementptr inbounds ([15 x i8], [15 x i8]* @str, i64 0, i64 0))
  %12 = load i32, i32* %arraydecay, align 16, !tbaa !2
  %call.i = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %12) #6
  %arrayidx.i5.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  %13 = load i32, i32* %arrayidx.i5.1, align 4, !tbaa !2
  %call.i.1 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %13) #6
  %arrayidx.i5.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  %14 = load i32, i32* %arrayidx.i5.2, align 8, !tbaa !2
  %call.i.2 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %14) #6
  %arrayidx.i5.3 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  %15 = load i32, i32* %arrayidx.i5.3, align 4, !tbaa !2
  %call.i.3 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %15) #6
  %arrayidx.i5.4 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 4
  %16 = load i32, i32* %arrayidx.i5.4, align 16, !tbaa !2
  %call.i.4 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %16) #6
  %arrayidx.i5.5 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 5
  %17 = load i32, i32* %arrayidx.i5.5, align 4, !tbaa !2
  %call.i.5 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %17) #6
  %arrayidx.i5.6 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 6
  %18 = load i32, i32* %arrayidx.i5.6, align 8, !tbaa !2
  %call.i.6 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %18) #6
  %putchar.i = tail call i32 @putchar(i32 10) #6
  call void @llvm.lifetime.end.p0i8(i64 28, i8* nonnull %0) #6
  ret i32 0

if.then.i.4.1:                                    ; preds = %for.inc.i.4
  %arrayidx.i.4.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  store i32 %11, i32* %arrayidx.i.4.1, align 4, !tbaa !2
  store i32 %10, i32* %arrayidx7.i.4.1, align 8, !tbaa !2
  br label %for.inc.i.4.1

for.inc.i.4.1:                                    ; preds = %if.then.i.4.1, %for.inc.i.4
  %.pre.i.5 = load i32, i32* %arraydecay, align 16, !tbaa !2
  %arrayidx7.i.5 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  %19 = load i32, i32* %arrayidx7.i.5, align 4, !tbaa !2
  %cmp8.i.5 = icmp sgt i32 %.pre.i.5, %19
  br i1 %cmp8.i.5, label %if.then.i.5, label %for.inc14.i.5

if.then.i.3.1:                                    ; preds = %for.inc.i.3
  %arrayidx.i.3.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  store i32 %9, i32* %arrayidx.i.3.1, align 4, !tbaa !2
  store i32 %8, i32* %arrayidx7.i.3.1, align 8, !tbaa !2
  br label %for.inc.i.3.1

for.inc.i.3.1:                                    ; preds = %if.then.i.3.1, %for.inc.i.3
  %20 = phi i32 [ %9, %for.inc.i.3 ], [ %8, %if.then.i.3.1 ]
  %arrayidx7.i.3.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  %21 = load i32, i32* %arrayidx7.i.3.2, align 4, !tbaa !2
  %cmp8.i.3.2 = icmp sgt i32 %20, %21
  br i1 %cmp8.i.3.2, label %if.then.i.3.2, label %for.inc.i.3.2

if.then.i.3.2:                                    ; preds = %for.inc.i.3.1
  %arrayidx.i.3.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  store i32 %21, i32* %arrayidx.i.3.2, align 8, !tbaa !2
  store i32 %20, i32* %arrayidx7.i.3.2, align 4, !tbaa !2
  br label %for.inc.i.3.2

for.inc.i.3.2:                                    ; preds = %if.then.i.3.2, %for.inc.i.3.1
  %.pre.i.4 = load i32, i32* %arraydecay, align 16, !tbaa !2
  %arrayidx7.i.4 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  %22 = load i32, i32* %arrayidx7.i.4, align 4, !tbaa !2
  %cmp8.i.4 = icmp sgt i32 %.pre.i.4, %22
  br i1 %cmp8.i.4, label %if.then.i.4, label %for.inc.i.4

if.then.i.2.1:                                    ; preds = %for.inc.i.2
  %arrayidx.i.2.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  store i32 %7, i32* %arrayidx.i.2.1, align 4, !tbaa !2
  store i32 %6, i32* %arrayidx7.i.2.1, align 8, !tbaa !2
  br label %for.inc.i.2.1

for.inc.i.2.1:                                    ; preds = %if.then.i.2.1, %for.inc.i.2
  %23 = phi i32 [ %7, %for.inc.i.2 ], [ %6, %if.then.i.2.1 ]
  %arrayidx7.i.2.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  %24 = load i32, i32* %arrayidx7.i.2.2, align 4, !tbaa !2
  %cmp8.i.2.2 = icmp sgt i32 %23, %24
  br i1 %cmp8.i.2.2, label %if.then.i.2.2, label %for.inc.i.2.2

if.then.i.2.2:                                    ; preds = %for.inc.i.2.1
  %arrayidx.i.2.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  store i32 %24, i32* %arrayidx.i.2.2, align 8, !tbaa !2
  store i32 %23, i32* %arrayidx7.i.2.2, align 4, !tbaa !2
  br label %for.inc.i.2.2

for.inc.i.2.2:                                    ; preds = %if.then.i.2.2, %for.inc.i.2.1
  %25 = phi i32 [ %24, %for.inc.i.2.1 ], [ %23, %if.then.i.2.2 ]
  %arrayidx7.i.2.3 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 4
  %26 = load i32, i32* %arrayidx7.i.2.3, align 16, !tbaa !2
  %cmp8.i.2.3 = icmp sgt i32 %25, %26
  br i1 %cmp8.i.2.3, label %if.then.i.2.3, label %for.inc.i.2.3

if.then.i.2.3:                                    ; preds = %for.inc.i.2.2
  %arrayidx.i.2.3 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  store i32 %26, i32* %arrayidx.i.2.3, align 4, !tbaa !2
  store i32 %25, i32* %arrayidx7.i.2.3, align 16, !tbaa !2
  br label %for.inc.i.2.3

for.inc.i.2.3:                                    ; preds = %if.then.i.2.3, %for.inc.i.2.2
  %.pre.i.3 = load i32, i32* %arraydecay, align 16, !tbaa !2
  %arrayidx7.i.3 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  %27 = load i32, i32* %arrayidx7.i.3, align 4, !tbaa !2
  %cmp8.i.3 = icmp sgt i32 %.pre.i.3, %27
  br i1 %cmp8.i.3, label %if.then.i.3, label %for.inc.i.3

if.then.i.1.1:                                    ; preds = %for.inc.i.1
  %arrayidx.i.1.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  store i32 %5, i32* %arrayidx.i.1.1, align 4, !tbaa !2
  store i32 %4, i32* %arrayidx7.i.1.1, align 8, !tbaa !2
  br label %for.inc.i.1.1

for.inc.i.1.1:                                    ; preds = %if.then.i.1.1, %for.inc.i.1
  %28 = phi i32 [ %5, %for.inc.i.1 ], [ %4, %if.then.i.1.1 ]
  %arrayidx7.i.1.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  %29 = load i32, i32* %arrayidx7.i.1.2, align 4, !tbaa !2
  %cmp8.i.1.2 = icmp sgt i32 %28, %29
  br i1 %cmp8.i.1.2, label %if.then.i.1.2, label %for.inc.i.1.2

if.then.i.1.2:                                    ; preds = %for.inc.i.1.1
  %arrayidx.i.1.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  store i32 %29, i32* %arrayidx.i.1.2, align 8, !tbaa !2
  store i32 %28, i32* %arrayidx7.i.1.2, align 4, !tbaa !2
  br label %for.inc.i.1.2

for.inc.i.1.2:                                    ; preds = %if.then.i.1.2, %for.inc.i.1.1
  %30 = phi i32 [ %29, %for.inc.i.1.1 ], [ %28, %if.then.i.1.2 ]
  %arrayidx7.i.1.3 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 4
  %31 = load i32, i32* %arrayidx7.i.1.3, align 16, !tbaa !2
  %cmp8.i.1.3 = icmp sgt i32 %30, %31
  br i1 %cmp8.i.1.3, label %if.then.i.1.3, label %for.inc.i.1.3

if.then.i.1.3:                                    ; preds = %for.inc.i.1.2
  %arrayidx.i.1.3 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  store i32 %31, i32* %arrayidx.i.1.3, align 4, !tbaa !2
  store i32 %30, i32* %arrayidx7.i.1.3, align 16, !tbaa !2
  br label %for.inc.i.1.3

for.inc.i.1.3:                                    ; preds = %if.then.i.1.3, %for.inc.i.1.2
  %32 = phi i32 [ %31, %for.inc.i.1.2 ], [ %30, %if.then.i.1.3 ]
  %arrayidx7.i.1.4 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 5
  %33 = load i32, i32* %arrayidx7.i.1.4, align 4, !tbaa !2
  %cmp8.i.1.4 = icmp sgt i32 %32, %33
  br i1 %cmp8.i.1.4, label %if.then.i.1.4, label %for.inc.i.1.4

if.then.i.1.4:                                    ; preds = %for.inc.i.1.3
  %arrayidx.i.1.4 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 4
  store i32 %33, i32* %arrayidx.i.1.4, align 16, !tbaa !2
  store i32 %32, i32* %arrayidx7.i.1.4, align 4, !tbaa !2
  br label %for.inc.i.1.4

for.inc.i.1.4:                                    ; preds = %if.then.i.1.4, %for.inc.i.1.3
  %.pre.i.2 = load i32, i32* %arraydecay, align 16, !tbaa !2
  %arrayidx7.i.2 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  %34 = load i32, i32* %arrayidx7.i.2, align 4, !tbaa !2
  %cmp8.i.2 = icmp sgt i32 %.pre.i.2, %34
  br i1 %cmp8.i.2, label %if.then.i.2, label %for.inc.i.2

if.then.i.112:                                    ; preds = %for.inc.i
  %arrayidx.i.111 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  store i32 %3, i32* %arrayidx.i.111, align 4, !tbaa !2
  store i32 %2, i32* %arrayidx7.i.18, align 8, !tbaa !2
  br label %for.inc.i.113

for.inc.i.113:                                    ; preds = %if.then.i.112, %for.inc.i
  %35 = phi i32 [ %3, %for.inc.i ], [ %2, %if.then.i.112 ]
  %arrayidx7.i.214 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  %36 = load i32, i32* %arrayidx7.i.214, align 4, !tbaa !2
  %cmp8.i.215 = icmp sgt i32 %35, %36
  br i1 %cmp8.i.215, label %if.then.i.218, label %for.inc.i.219

if.then.i.218:                                    ; preds = %for.inc.i.113
  %arrayidx.i.217 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 2
  store i32 %36, i32* %arrayidx.i.217, align 8, !tbaa !2
  store i32 %35, i32* %arrayidx7.i.214, align 4, !tbaa !2
  br label %for.inc.i.219

for.inc.i.219:                                    ; preds = %if.then.i.218, %for.inc.i.113
  %37 = phi i32 [ %36, %for.inc.i.113 ], [ %35, %if.then.i.218 ]
  %arrayidx7.i.320 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 4
  %38 = load i32, i32* %arrayidx7.i.320, align 16, !tbaa !2
  %cmp8.i.321 = icmp sgt i32 %37, %38
  br i1 %cmp8.i.321, label %if.then.i.324, label %for.inc.i.325

if.then.i.324:                                    ; preds = %for.inc.i.219
  %arrayidx.i.323 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 3
  store i32 %38, i32* %arrayidx.i.323, align 4, !tbaa !2
  store i32 %37, i32* %arrayidx7.i.320, align 16, !tbaa !2
  br label %for.inc.i.325

for.inc.i.325:                                    ; preds = %if.then.i.324, %for.inc.i.219
  %39 = phi i32 [ %38, %for.inc.i.219 ], [ %37, %if.then.i.324 ]
  %arrayidx7.i.426 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 5
  %40 = load i32, i32* %arrayidx7.i.426, align 4, !tbaa !2
  %cmp8.i.427 = icmp sgt i32 %39, %40
  br i1 %cmp8.i.427, label %if.then.i.430, label %for.inc.i.431

if.then.i.430:                                    ; preds = %for.inc.i.325
  %arrayidx.i.429 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 4
  store i32 %40, i32* %arrayidx.i.429, align 16, !tbaa !2
  store i32 %39, i32* %arrayidx7.i.426, align 4, !tbaa !2
  br label %for.inc.i.431

for.inc.i.431:                                    ; preds = %if.then.i.430, %for.inc.i.325
  %41 = phi i32 [ %40, %for.inc.i.325 ], [ %39, %if.then.i.430 ]
  %arrayidx7.i.532 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 6
  %42 = load i32, i32* %arrayidx7.i.532, align 8, !tbaa !2
  %cmp8.i.533 = icmp sgt i32 %41, %42
  br i1 %cmp8.i.533, label %if.then.i.535, label %for.inc.i.5

if.then.i.535:                                    ; preds = %for.inc.i.431
  %arrayidx.i.534 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 5
  store i32 %42, i32* %arrayidx.i.534, align 4, !tbaa !2
  store i32 %41, i32* %arrayidx7.i.532, align 8, !tbaa !2
  br label %for.inc.i.5

for.inc.i.5:                                      ; preds = %if.then.i.535, %for.inc.i.431
  %.pre.i.1 = load i32, i32* %arraydecay, align 16, !tbaa !2
  %arrayidx7.i.1 = getelementptr inbounds [7 x i32], [7 x i32]* %arr, i64 0, i64 1
  %43 = load i32, i32* %arrayidx7.i.1, align 4, !tbaa !2
  %cmp8.i.1 = icmp sgt i32 %.pre.i.1, %43
  br i1 %cmp8.i.1, label %if.then.i.1, label %for.inc.i.1
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: nofree nounwind
declare i32 @putchar(i32) local_unnamed_addr #5

; Function Attrs: nofree nounwind
declare i32 @puts(i8* nocapture readonly) local_unnamed_addr #5

attributes #0 = { nofree norecurse nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { nofree nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nofree nounwind }
attributes #6 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0 (https://github.com/llvm/llvm-project.git c2ca003baffbdb58b4a2e5c9c5163122844a82d3)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"int", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
