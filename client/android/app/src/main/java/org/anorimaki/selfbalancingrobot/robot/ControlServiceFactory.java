package org.anorimaki.selfbalancingrobot.robot;

import retrofit2.Retrofit;
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.jackson.JacksonConverterFactory;

public class ControlServiceFactory {
	public static ControlService create(RobotConfig config ) {
        Retrofit retrofit = new Retrofit.Builder().
				baseUrl(config.getUrl().toString()).
				addConverterFactory(JacksonConverterFactory.create()).
				addCallAdapterFactory(RxJava2CallAdapterFactory.create()).
				build();

		return retrofit.create(ControlService.class);
	}
}
