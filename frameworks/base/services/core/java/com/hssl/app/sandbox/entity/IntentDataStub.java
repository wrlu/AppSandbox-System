package com.hssl.app.sandbox.entity;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Closeable;
import java.util.List;

import android.content.Intent;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Parcel;

public class IntentDataStub {
    private static final String TAG = "IntentDataStub";

    private int uid;
    private int pid;
    private long timestamp;
    private String operation;
    private String action;
    private String data;
    private String type;
    private String identifier;
    private String packageName;
    private String componentName;
    private int flags;
    private String categories;
    private String clipData;
    private String extra;

    public static IntentDataStub fromIntent(Intent intent) {
        IntentDataStub intentData = new IntentDataStub();
        intentData.action = intent.getAction();
        intentData.data = intent.getDataString();
        intentData.type = intent.getType();
        intentData.identifier = intent.getIdentifier();
        intentData.packageName = intent.getPackage();
        if (intent.getComponent() != null) {
            intentData.componentName = intent.getComponent().flattenToShortString();
        }
        intentData.flags = intent.getFlags();
        if (intent.getCategories() != null) {
            StringBuilder categories = new StringBuilder();
            for (String category : intent.getCategories()) {
                categories.append(category);
                categories.append(" ");
            }
            intentData.categories = categories.toString();
        }
        if (intent.getClipData() != null) {
            StringBuilder clipData = new StringBuilder();
            for (int i = 0; i < intent.getClipData().getItemCount(); ++i) {
                clipData.append(intent.getClipData().getItemAt(i).toString());
                clipData.append(" ");
            }
            intentData.clipData = clipData.toString();
        }
        if (intent.getExtras() != null) {
            StringBuilder extras = new StringBuilder();
            for (String key : intent.getExtras().keySet()) {
                extras.append(key);
                extras.append(" ");
            }
            intentData.extra = extras.toString();
        }
        return intentData;
    }

    public static LocalSocket openSocket() throws IOException {
        LocalSocket ls = new LocalSocket();
        ls.connect(new LocalSocketAddress("AppSandbox_Intent"));
        return ls;
    }

    public void writeStream(OutputStream os) {
        if (os == null) {
            return;
        }
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(os));
        try {
            writer.write(String.valueOf(uid));
            writer.newLine();
            writer.write(String.valueOf(pid));
            writer.newLine();
            writer.write(String.valueOf(timestamp));
            writer.newLine();
            if (operation != null) {
                writer.write(operation);
            }
            writer.newLine();
            if (action != null) {
                writer.write(action);
            }
            writer.newLine();
            if (data != null) {
                writer.write(data);
            }
            writer.newLine();
            if (type != null) {
                writer.write(type);
            }
            writer.newLine();
            if (identifier != null) {
                writer.write(identifier);
            }
            writer.newLine();
            if (packageName != null) {
                writer.write(packageName);
            }
            writer.newLine();
            if (componentName != null) {
                writer.write(componentName);
            }
            writer.newLine();
            writer.write(String.valueOf(flags));
            writer.newLine();
            if (categories != null) {
                writer.write(categories);
            }
            writer.newLine();
            if (clipData != null) {
                writer.write(clipData);
            }
            writer.newLine();
            if (extra != null) {
                writer.write(extra);
            }
            writer.newLine();
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public int getUid() {
        return uid;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public int getPid() {
        return pid;
    }

    public void setPid(int pid) {
        this.pid = pid;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public String getOperation() {
        return operation;
    }

    public void setOperation(String operation) {
        this.operation = operation;
    }

    public String getAction() {
        return action;
    }

    public void setAction(String action) {
        this.action = action;
    }

    public String getData() {
        return data;
    }

    public void setData(String data) {
        this.data = data;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getIdentifier() {
        return identifier;
    }

    public void setIdentifier(String identifier) {
        this.identifier = identifier;
    }

    public String getPackageName() {
        return packageName;
    }

    public void setPackageName(String packageName) {
        this.packageName = packageName;
    }

    public String getComponentName() {
        return componentName;
    }

    public void setComponentName(String componentName) {
        this.componentName = componentName;
    }

    public int getFlags() {
        return flags;
    }

    public void setFlags(int flags) {
        this.flags = flags;
    }

    public String getCategories() {
        return categories;
    }

    public void setCategories(String categories) {
        this.categories = categories;
    }

    public String getClipData() {
        return clipData;
    }

    public void setClipData(String clipData) {
        this.clipData = clipData;
    }

    public String getExtra() {
        return extra;
    }

    public void setExtra(String extra) {
        this.extra = extra;
    }
}
