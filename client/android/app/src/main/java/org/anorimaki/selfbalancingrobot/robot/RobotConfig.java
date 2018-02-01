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
            this.url = new URL(properties.getProperty("robot.url"));
        }
        catch( Exception e ) {
            this.url = null;            //Leave empty URL
        }
    }

    public URL getURL() {
        return url;
    }
}
