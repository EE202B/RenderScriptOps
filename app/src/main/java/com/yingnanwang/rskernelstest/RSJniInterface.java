package com.yingnanwang.rskernelstest;

/**
 * Created by YingnanWang on 3/12/17.
 */

public class RSJniInterface {
    static{
        try {
            System.loadLibrary("rsKernels_jni");
        } catch (UnsatisfiedLinkError e) {
            throw new RuntimeException(
                    e.toString());

        }
    }

    public static native void sgemmTest(String cacheDir);
}
