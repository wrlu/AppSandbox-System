/* GENERATED SOURCE. DO NOT MODIFY. */
/*
 * Copyright (C) 2014 Square, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.okhttp.okio;

import java.io.IOException;
import java.io.FileOutputStream;
import java.lang.reflect.Field;
import com.android.okhttp.internal.http.HttpEngine;
import com.android.okhttp.internal.http.Http2xStream;
import com.hssl.app.sandbox.entity.HttpDataStub;

/** A {@link Source} which forwards calls to another. Useful for subclassing. 
 * @hide This class is not part of the Android public SDK API*/
public abstract class ForwardingSource implements Source {
  private final Source delegate;

  public ForwardingSource(Source delegate) {
    if (delegate == null) throw new IllegalArgumentException("delegate == null");
    this.delegate = delegate;
  }

  /** {@link Source} to which this instance is delegating. */
  public final Source delegate() {
    return delegate;
  }

  @Override public long read(Buffer sink, long byteCount) throws IOException {
    long read = delegate.read(sink, byteCount);
    //Modifications here
    Class<?> clazz = this.getClass();
    Field streamField = null;
    try {
      streamField = clazz.getDeclaredField("this$0");
      Http2xStream http2xStream = null;
      http2xStream = (Http2xStream)streamField.get(this);
      Class<?> clazz2 = http2xStream.getClass();
      Field hField = clazz2.getDeclaredField("httpEngine");
      hField.setAccessible(true);
      HttpEngine httpEngine = (com.android.okhttp.internal.http.HttpEngine) hField.get(http2xStream);
      if(!httpEngine.okOutput && httpEngine.localSocket != null){
        HttpDataStub.writeBody(HttpDataStub.getOutputStream(httpEngine.localSocket), sink.clone().readByteArray(), 0, (int)read);
      }else{
        throw new IllegalStateException("unexcepted bodyrecording status");
      }
    } catch (IllegalAccessException e) {
      e.printStackTrace();
    } catch (NoSuchFieldException e) {
      e.printStackTrace();
    }

    return read;
  }

  @Override public Timeout timeout() {
    return delegate.timeout();
  }

  @Override public void close() throws IOException {
    delegate.close();
  }

  @Override public String toString() {
    return getClass().getSimpleName() + "(" + delegate.toString() + ")";
  }
}
