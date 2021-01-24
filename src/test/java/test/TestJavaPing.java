package test;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

public class TestJavaPing {


    @BeforeAll
    static void before() {
        System.out.println("before");
    }


    @Test
    void testPingError() {
        JavaPing.PingError e1 = JavaPing.PingError.valueOf(1);
        Assertions.assertEquals(e1, JavaPing.PingError.PING_FAILED);
    }


    @Test
    void testPingInfo() {
        JavaPing.PingError e = JavaPing.PingError.PING_OK;
        final String i = JavaPing.getErrorDesc(0);
        Assertions.assertEquals(e.info, i);
    }


    @Test
    void testPingHost() {
        int res = JavaPing.ping4("8.8.8.8", 1);
        Assertions.assertEquals(res, 0);
    }

    @Test
    void testPingBadHost() {
        int res = JavaPing.ping4("8.8.8.899", 1);
        Assertions.assertNotEquals(res, 0);
    }

}
