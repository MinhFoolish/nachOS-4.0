class PCB{
private:
    Semaphore *joinsem; // Semaphore for join process.
    Semaphore *exitsem; // Semaphore for exit process.
    Semaphore *multex;  // Độc quyền truy xuất cho các quá trình.

    int exitcode;
    int numwait;  // Ready queue.
public:
    int parentID;
    PCB(int id);
    ~PCB();

    int Exec(char *filename, int pid);  // Hàm thực thi chương trình có tên là filename và process ID pid.
    int GetID();  // return pid of current process.
    int GetNumWait(); // return number of process in ready queue.

    void JoinWait();  // Parent process wait for the child process finishes.
    void ExitWait();  // The child process finishes
    void JoinRelease(); // The child process notice the parent process.
    void ExitRelease(); // The parent process accept to the exit child process.

    void IncNumWait();  // Tăng số lượng tiến trình đợi lên.
    void DecNumWait();  // Giảm đi.

    void SetExitCode(int ec); // Thiết lập mã thoát cho tiến trình.
    int GetExitCode();  // Lấy thông tin mã thoát.

    void SetFileName(char *fn); // Thiết lập tên file.
    char* GetFileName();  // Lấy tên file.
};