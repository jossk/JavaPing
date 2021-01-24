package test;

public class JavaPing {

    static {
        System.loadLibrary("JavaPing");
    }

    /// main procedure - ping hostname with timeout in seconds
    public static native int ping4(final String hostname, int timeoutSec);

    // see ping.h for errors
    public static enum PingError {
        PING_OK(0, "Ping is OK"),
        PING_FAILED(1, "Ping is FAIL!"),
        PING_ERROR_INVALID_HOSTNAME(2, "Invalid hostname"),
        PING_ERROR_GETPROTOBYNAME(3, "Can not ICMP protocol info"),
        PING_ERROR_GETHOSTBYNAME(4, "Can not get address by hostname"),
        PING_ERROR_SOCKET_OPEN(5, "Socket open error"),
        PING_ERROR_SOCKET_SEND(6, "Socket send error"),
        PING_ERROR_SOCKET_RECV(7, "Socket recv error"),
        PING_ERROR_INVALID_RECV(8, "Socket recv invalid frame");

        final int code;
        final String info;

        PingError(int code, final String info) {
            this.code = code;
            this.info = info;
        }

        static PingError valueOf(final int error) {
            for (PingError e : values()) {
                if (e.code == error) {
                    return e;
                }
            }
            return  PING_FAILED;
        }

    };

    // get ping error description by ping's error code
    public static String getErrorDesc(final int error) {
        final PingError pingError = PingError.valueOf(error);
        return pingError.info;
    }

// for test
//    public static void main(String[] args) {
//        System.out.println("result:" + ping4("8.8.8.8", 1));
//    }
}
