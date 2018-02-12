package org.anorimaki.selfbalancingrobot.control;


import android.util.Log;
import android.widget.Toast;

import org.anorimaki.selfbalancingrobot.robot.ControlService;
import org.anorimaki.selfbalancingrobot.robot.ControlServiceFactory;
import org.anorimaki.selfbalancingrobot.robot.RobotConfig;
import org.anorimaki.selfbalancingrobot.robot.Targets;

import io.reactivex.Flowable;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

public class ControlPresenter implements ControlContract.Presenter {
    private static final String TAG = ControlPresenter.class.getSimpleName();

    private static int pidBits = 14;
    private static int maxPidValue = (1<<pidBits)-1;

    private Flowable<ControlContract.JoystickMove> userMoves;
    private Disposable targetsSubscription;
    private ControlService service;

    public ControlPresenter( RobotConfig robotConfig ) {
        this.service = ControlServiceFactory.create( robotConfig );
    }

    @Override
    public void attach(ControlContract.View view) {
        targetsSubscription = view.getTargets().
                subscribeOn(Schedulers.io()).
                observeOn(Schedulers.io()).
                flatMap( move -> {
                    double angle = move.getAngle() * Math.PI / 180;     //To rads
                    double speed = Math.sin( angle ) * move.getStrength();
                    double heading = Math.cos( angle ) * move.getStrength();

                    speed = (maxPidValue*speed) / 100;     //Scale value
                    heading = (maxPidValue*heading) / 100;     //Scale value

                    return service.setTargets( new Targets( (int)speed, (int)heading ) ).toFlowable();
                } ).
                observeOn(AndroidSchedulers.mainThread()).
                doOnError( error -> {
                    view.showError( "Error setting target" );
                }).
                retry().
                subscribe( response -> {
                        Log.d(TAG, Thread.currentThread().getId() + ": response ok" + response);
                    },
                    error -> {
                        Log.e(TAG, Thread.currentThread().getId() + ": response error", error);
                    });
    }

    @Override
    public void detach() {
        targetsSubscription.dispose();
    }
}
