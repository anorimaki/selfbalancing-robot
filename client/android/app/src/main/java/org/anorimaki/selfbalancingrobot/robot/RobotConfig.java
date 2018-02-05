package org.anorimaki.selfbalancingrobot.robot;

import android.content.Context;

import org.anorimaki.selfbalancingrobot.R;
import org.anorimaki.selfbalancingrobot.util.PropetiesHelper;

import java.io.IOException;
import java.net.URL;
import java.util.Properties;

/**
 * Created by david.osorio on 31/01/2018.
 */

public class RobotConfig {
    private URL url;

    public RobotConfig( Context context ) {
        try {
            Properties properties = PropetiesHelper.load(context, R.raw.app);
            String baseUrl = properties.getProperty("robot.url");
            if ( baseUrl == null ) {
                return;
            }
            this.url = new URL(baseUrl);
        }
        catch( Exception e ) {
            this.url = null;
        }
    }

    public URL getUrl() {
        return url;
    }
}
