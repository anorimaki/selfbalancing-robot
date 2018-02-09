package org.anorimaki.selfbalancingrobot.control;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.Toast;

import org.anorimaki.selfbalancingrobot.R;
import org.anorimaki.selfbalancingrobot.robot.RobotConfig;

import io.github.controlwear.virtual.joystick.android.JoystickView;
import io.reactivex.BackpressureStrategy;
import io.reactivex.Flowable;

public class ControlActivity extends AppCompatActivity implements ControlContract.View{
	private ControlContract.Presenter presenter;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
	    inject();

		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_control);
	}

	@Override
    protected void onResume() {
	    super.onResume();

        presenter.attach( this );
    }

    @Override
    protected void onPause() {
	    super.onPause();

        presenter.detach();
    }

    @Override
    public void showError( String msg ) {
        Toast toast = Toast.makeText(this, msg, Toast.LENGTH_SHORT );
        toast.show();
    }

    @Override
    public Flowable<ControlContract.JoystickMove> getTargets() {
		return Flowable.create( emitter -> {
                JoystickView joystick = findViewById(R.id.joystickView);
                emitter.setCancellable( () -> {
                    joystick.setOnMoveListener(null);
                });

                joystick.setOnMoveListener( (int angle, int strength) -> {
                    if (!emitter.isCancelled()) {
                        emitter.onNext(new ControlContract.JoystickMove(angle, strength));
                    }
                }, 500 );       //Generate events every 500ms
            }, BackpressureStrategy.LATEST );
	}

    private void inject() {
        //TODO: Replace by Dagger
        presenter = new ControlPresenter(new RobotConfig(getApplicationContext()));
    }
}
