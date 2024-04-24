/*
 * Copyright (C) 2014 The Android Open Source Project
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

package com.android.terminal;

import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.util.Log;
import android.view.MenuItem;

import static com.android.terminal.Terminal.TAG;

import com.wrlus.app.terminal.R;

/**
 * Settings for Terminal.
 */
public class TerminalSettingsActivity extends PreferenceActivity {

    public static final String KEY_FULLSCREEN_MODE = "fullscreen_mode";
    public static final String KEY_SCREEN_ORIENTATION = "screen_orientation";
    public static final String KEY_FONT_SIZE = "font_size";
    public static final String KEY_TEXT_COLORS = "text_colors";
    public static final String KEY_VOLUME_MODE = "volumekey_mode";

    private SwitchPreference mFullscreenModePref;
    private ListPreference mScreenOrientationPref;
    private ListPreference mFontSizePref;
    private ListPreference mTextColorsPref;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.settings);

        mFullscreenModePref = (SwitchPreference) findPreference(KEY_FULLSCREEN_MODE);
        mScreenOrientationPref = (ListPreference) findPreference(KEY_SCREEN_ORIENTATION);
        mFontSizePref = (ListPreference) findPreference(KEY_FONT_SIZE);
        mTextColorsPref = (ListPreference) findPreference(KEY_TEXT_COLORS);

        getActionBar().setDisplayHomeAsUpEnabled(true);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
