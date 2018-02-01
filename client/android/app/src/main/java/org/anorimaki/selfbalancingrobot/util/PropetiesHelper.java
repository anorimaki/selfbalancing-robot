/*
 * Copyright (c) 1999-2017 Safelayer Secure Communications, S.A.
 *
 *  All rights reserved. No part of this source code may be reproduced,
 *  stored in a retrieval system, or transmitted, in any form or by any
 *  means, electronic, mechanical, photocopying, recording or otherwise,
 *  except as in the end-user license agreement, without the prior
 *  permission of the copyright owner.
 */

package org.anorimaki.selfbalancingrobot.util;

import android.content.Context;
import android.content.res.Resources;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

public class PropetiesHelper {
    public static Properties load(Context context, int idFile) throws IOException {
        Resources resources = context.getResources();
        InputStream rawResource = resources.openRawResource(idFile);
        Properties properties = new Properties();
        properties.load(rawResource);
        return properties;
    }
}
