package org.anorimaki.selfbalancingrobot.control;


import android.util.Log;
import android.widget.Toast;

import org.anorimaki.selfbalancingrobot.R;
import org.anorimaki.selfbalancingrobot.robot.ControlService;
import org.anorimaki.selfbalancingrobot.robot.ControlServiceFactory;
import org.anorimaki.selfbalancingrobot.robot.RobotConfig;
import org.anorimaki.selfbalancingrobot.robot.Targets;

import javax.inject.Inject;

import io.reactivex.Flowable;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

public class ControlPresenter implements ControlContract.Presenter {
    private static final String TAG = ControlPresenter.class.getSimpleName();

    private static int pidBits = 15;        //Signed
    private static int maxPidValue = (1<<(pidBits-1))-1;

    private static int maxSpeed = (maxPidValue*25)/100;         //No more than 25%
    private static int maxHeading = (maxPidValue*25)/100;       //No more than 25%


    private Flowable<ControlContract.JoystickMove> userMoves;
    private Disposable targetsSubscription;
    private ControlService service;

    @Inject
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

                    speed = (maxSpeed*speed) / 100;     //Scale value
                    heading = (maxHeading*heading) / 100;     //Scale value

                    long startTime = System.currentTimeMillis();

                    return service.setTargets( new Targets( (int)speed, (int)heading ) ).
                            toSingle( () -> System.currentTimeMillis()-startTime ).
                            toFlowable();
                } ).
                observeOn(AndroidSchedulers.mainThread()).
                doOnError( error -> {
                    view.showSettingTargetsError();
                    Log.e( TAG, "Error setting target", error );
                }).
                retry().
                subscribe( responseTime -> {
                        view.showResponseTime(responseTime);
                    });
    }

    @Override
    public void detach() {
        targetsSubscription.dispose();
    }
}
