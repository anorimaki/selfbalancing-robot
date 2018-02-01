package org.anorimaki.selfbalancingrobot.robot;

import retrofit2.Retrofit;
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.jackson.JacksonConverterFactory;

/**
 * Created by david.osorio on 31/01/2018.
 */

public class ControlServiceFactory {
	public static ControlService create(RobotConfig config ) {
        Retrofit retrofit = new Retrofit.Builder().
				baseUrl(config.getURL().toString()).
				addConverterFactory(JacksonConverterFactory.create()).
				addCallAdapterFactory(RxJava2CallAdapterFactory.create()).
				build();

		return retrofit.create(ControlService.class);
	}
}
