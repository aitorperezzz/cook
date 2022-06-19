; declare global variable that stores the memory
@globalBuffer = private global [30000 x i8] zeroinitializer

; declare global variable as pointer to i64
@globalPointer = private global i8* zeroinitializer

; define main function
define i64 @main() {
    ; make global pointer point to the beginning of the global buffer
    %globalPointer = bitcast [30000 x i8]* @globalBuffer to i8*

    ; call the user-defined code
    call void @code()

    ; Exit with success
    ret i64 0
}

define void @code() {
    {{ user_code }}
}