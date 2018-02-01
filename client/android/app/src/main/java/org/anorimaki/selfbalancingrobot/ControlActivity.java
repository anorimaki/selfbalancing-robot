package org.anorimaki.selfbalancingrobot;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import org.anorimaki.selfbalancingrobot.robot.ControlService;
import org.anorimaki.selfbalancingrobot.robot.ControlServiceFactory;
import org.anorimaki.selfbalancingrobot.robot.RobotConfig;
import org.anorimaki.selfbalancingrobot.robot.Target;

import java.util.Observable;

import io.github.controlwear.virtual.joystick.android.JoystickView;
import io.reactivex.BackpressureStrategy;
import io.reactivex.Flowable;
import io.reactivex.Scheduler;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

public class ControlActivity extends AppCompatActivity {
    private static final String TAG = ControlActivity.class.getSimpleName();

	private static class JoystickMove {
		private int angle;
		private int strength;

		public JoystickMove( int angle, int strength ) {
			this.angle = angle;
			this.strength = strength;
		}

		public int getAngle() {
			return angle;
		}

		public int getStrength() {
			return strength;
		}
	}

	private Flowable<Object> setTargetResponses;
	private Disposable responsesSubscription;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_control);

        ControlService service = createControlService();

        setTargetResponses = getJoysticsMoves().
                subscribeOn(Schedulers.io()).
                observeOn(AndroidSchedulers.mainThread()).
                flatMap( move -> {
                    double angle = move.getAngle() * Math.PI / 180;
                    double speed = Math.sin( angle ) * move.getStrength();
                    double heading = Math.cos( angle ) * move.getStrength();
                    Log.d(TAG, Thread.currentThread().getId() + ": move " + speed + ", " + heading );
                    return service.setTarget( new Target( (int)speed, (int)heading ) ).toFlowable();
                } ).
                doOnError( error -> {
                    Log.e(TAG, Thread.currentThread().getId() + ": response error", error);
                }).
                retry();
	}

	@Override
    protected void onResume() {
	    super.onResume();

        responsesSubscription = setTargetResponses.subscribe( response -> {
                    Log.d(TAG, Thread.currentThread().getId() + ": response ok" + response);
                },
                error -> {
                    Log.e(TAG, Thread.currentThread().getId() + ": response error", error);
                });
    }

    @Override
    protected void onPause() {
	    super.onPause();

        responsesSubscription.dispose();
    }

	private ControlService createControlService() {
	    RobotConfig config = new RobotConfig(this);
        return ControlServiceFactory.create( config );
    }

	private Flowable<JoystickMove> getJoysticsMoves() {
		return Flowable.create( emitter -> {
                JoystickView joystick = findViewById(R.id.joystickView);
                emitter.setCancellable( () -> {
                    joystick.setOnMoveListener(null);
                });

                joystick.setOnMoveListener( (int angle, int strength) -> {
                    if (!emitter.isCancelled()) {
                        emitter.onNext(new JoystickMove(angle, strength));
                    }
                }, 500 );       //Generate events every 500ms
            }, BackpressureStrategy.LATEST );
	}
}
