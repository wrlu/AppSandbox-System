package com.hssl.app.sandbox.entity;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Closeable;
import java.util.List;

public class HttpDataStub {
    private static final String TAG = "HttpDataStub";

    private int uid;
    private int pid;
    private String packageName;
    private long timestamp;
    private String requestOrStatusLine;
    private List<String> headerLines;
    private String bodySaveFile;

    public static Closeable openSocket() throws IOException {
        try {
            Class<?> localSocketClass = Class.forName("android.net.LocalSocket");
            Class<?> localSocketAddressClass = Class.forName("android.net.LocalSocketAddress");
            Closeable localSocket =
                    (Closeable) localSocketClass.newInstance();
            Object localSocketAddr = localSocketAddressClass.getDeclaredConstructor(String.class)
                    .newInstance("AppSandbox_Http");
            localSocketClass.getDeclaredMethod("connect", localSocketAddressClass)
                    .invoke(localSocket, localSocketAddr);
            return localSocket;
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static OutputStream getOutputStream(Object localSocket) throws IOException {
        if (localSocket == null) {
            return null;
        }
        try {
            Class<?> localSocketClass = Class.forName("android.net.LocalSocket");
            return (OutputStream) localSocketClass.getDeclaredMethod("getOutputStream")
                    .invoke(localSocket);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void writeBasicInfo(OutputStream os) {
        if (os == null) {
            return;
        }
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(os));
        try {
            writer.write(String.valueOf(uid));
            writer.newLine();
            writer.write(String.valueOf(pid));
            writer.newLine();
            if (packageName != null) {
                writer.write(packageName);
            } else {
                writer.write("");
            }
            writer.newLine();
            writer.write(String.valueOf(timestamp));
            writer.newLine();
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void writeHeader(OutputStream os) {
        if (os == null) {
            return;
        }
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(os));
        try {
            if (requestOrStatusLine != null) {
                writer.write(requestOrStatusLine);
            } else {
                writer.write("");
            }
            writer.newLine();

            if (headerLines != null) {
                for (String headerLine : headerLines) {
                    writer.write(headerLine);
                    writer.newLine();
                }
            }
            writer.newLine();
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void writeBody(OutputStream os, byte[] buffer, int off, int len) {
        if (os == null || buffer == null) {
            return;
        }
        try {
            os.write(buffer, off, len);
            os.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static int myUid() {
        try {
            Class<?> processClass = Class.forName("android.os.Process");
            return (int)
                    processClass.getDeclaredMethod("myUid")
                    .invoke(null);
        } catch (ReflectiveOperationException | NullPointerException e) {
            e.printStackTrace();
        }
        return -1;
    }

    public static int myPid() {
        try {
            Class<?> processClass = Class.forName("android.os.Process");
            return (int)
                    processClass.getDeclaredMethod("myPid")
                            .invoke(null);
        } catch (ReflectiveOperationException | NullPointerException e) {
            e.printStackTrace();
        }
        return -1;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public void setPid(int pid) {
        this.pid = pid;
    }

    public void setPackageName(String packageName) {
        this.packageName = packageName;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public void setRequestOrStatusLine(String requestOrStatusLine) {
        this.requestOrStatusLine = requestOrStatusLine;
    }

    public void setHeaderLines(List<String> headerLines) {
        this.headerLines = headerLines;
    }

    public void setBodySaveFile(String bodySaveFile) {
        this.bodySaveFile = bodySaveFile;
    }
}
