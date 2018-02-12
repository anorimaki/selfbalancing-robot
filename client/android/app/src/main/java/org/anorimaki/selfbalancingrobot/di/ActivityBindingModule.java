package org.anorimaki.selfbalancingrobot.di;

import org.anorimaki.selfbalancingrobot.control.ControlActivity;
import org.anorimaki.selfbalancingrobot.control.ControlModule;

import dagger.Module;
import dagger.android.ContributesAndroidInjector;

@Module
public abstract class ActivityBindingModule {
    @ContributesAndroidInjector(modules = ControlModule.class)
    abstract ControlActivity controlActivity();
}
