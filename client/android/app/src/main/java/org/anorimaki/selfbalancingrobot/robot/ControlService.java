package org.anorimaki.selfbalancingrobot.robot;

import io.reactivex.Completable;
import retrofit2.http.Body;
import retrofit2.http.PUT;

/**
 * Created by david.osorio on 01/02/2018.
 */

public interface ControlService {
    @PUT("/rest/targets")
    Completable setTargets(@Body Targets targets );
}
