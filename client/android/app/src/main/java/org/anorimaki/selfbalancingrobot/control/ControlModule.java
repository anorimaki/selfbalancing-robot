package org.anorimaki.selfbalancingrobot.control;

import dagger.Binds;
import dagger.Module;
import dagger.android.ContributesAndroidInjector;


@Module
public abstract class ControlModule {
    @Binds
    abstract ControlContract.Presenter taskPresenter(ControlPresenter presenter);
}

