package org.anorimaki.selfbalancingrobot;

import dagger.android.AndroidInjector;
import dagger.android.DaggerApplication;
import org.anorimaki.selfbalancingrobot.di.DaggerApplicationComponent;

public class RobotApplication extends DaggerApplication {
    @Override
    protected AndroidInjector<? extends DaggerApplication> applicationInjector() {
        return DaggerApplicationComponent.builder().application(this).build();
    }
}

