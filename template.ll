; declare global variable that stores the memory
@globalBuffer = private global [30000 x i8] zeroinitializer

; define main function
define i32 @main() {
    ; set all the global buffer memory to zero
    call void @initializeGlobalBuffer()

    ; Exit with success
    ret i32 0
}

; define function to initialize global buffer
define void @initializeGlobalBuffer() {
    ; Get pointer to the global buffer
    %globalBufferPointer = getelementptr [30000 x i8], [30000 x i8]* @globalBuffer, i64 0, i64 0

    ; call function that will set the memory to zero
    call void @setToZero([30000 x i8]* @globalBufferPointer, 30000)
    ret void
}

define void @setToZero([30000 x i8]* %pointer, double %n) {
entry:
    ; directly branch into the loop part
    br label %loop

loop:
    ; if coming from the entry point, i will be 0; if coming from loop, i will be nexti
    %i = phi i32 [0 %entry], [%nexti, %loop]
    ; body of the loop, set the current memory address
    %calltmp = call double @putchard(double 4.200000e+01)
    ; increment of the loop
    %nexti = fadd i32 %i, 1

    ; check if we need to end the loop
    %lastIndex = i32 sub %n, 1 
    %equal = icmp ueq i32 %i, %n
    br i1 %equal, label %afterloop, label %loop

afterloop:
    ; function always returns void
    ret void
}
