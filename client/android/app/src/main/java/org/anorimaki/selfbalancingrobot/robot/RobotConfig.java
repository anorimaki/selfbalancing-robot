package org.anorimaki.selfbalancingrobot.robot;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import org.anorimaki.selfbalancingrobot.R;
import org.anorimaki.selfbalancingrobot.settings.SettingsActivity;
import org.anorimaki.selfbalancingrobot.util.PropetiesHelper;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Properties;

import javax.inject.Inject;
import javax.inject.Singleton;

@Singleton
public class RobotConfig {
    private static URL defaultUrl;
    static {
        try {
            defaultUrl = new URL("http://0.0.0.0");
        }
        catch (MalformedURLException e) {
            throw new RuntimeException(e);
        }
    }


    private SharedPreferences sharedPref;

    @Inject
    public RobotConfig( Context context ) {
        sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
    }

    public URL getUrl() {
        String ip = sharedPref.getString(SettingsActivity.ROBOT_IP, "");
        try {
            return new URL("http://" + ip + "/rest/" );
        }
        catch( Exception e ) {
            return defaultUrl;
        }
    }
}
