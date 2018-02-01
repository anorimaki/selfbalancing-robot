package org.anorimaki.selfbalancingrobot.robot;

import io.reactivex.Completable;
import retrofit2.http.Body;
import retrofit2.http.POST;

/**
 * Created by david.osorio on 01/02/2018.
 */

public interface ControlService {
    @POST("target")
    Completable setTarget(@Body Target target );
}
