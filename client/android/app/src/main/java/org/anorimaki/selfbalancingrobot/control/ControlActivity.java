package org.anorimaki.selfbalancingrobot.control;

import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import org.anorimaki.selfbalancingrobot.R;

import javax.inject.Inject;

import dagger.android.support.DaggerAppCompatActivity;
import io.github.controlwear.virtual.joystick.android.JoystickView;
import io.reactivex.BackpressureStrategy;
import io.reactivex.Flowable;

public class ControlActivity extends DaggerAppCompatActivity implements ControlContract.View{
    @Inject
	ControlContract.Presenter presenter;

    private TextView resposeTimeView;

    @Inject
    public ControlActivity() {
        //Empty constructor needed by Dagger
    }

	@Override
	protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
		setContentView(R.layout.control_act);

        resposeTimeView = findViewById(R.id.response_time);
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
    public void showSettingTargetsError() {
        Toast toast = Toast.makeText(this, getString(R.string.setting_targets_error),
                                Toast.LENGTH_SHORT );
        toast.show();
    }

    @Override
    public void showResponseTime( long responseTime ) {
        resposeTimeView.setText( Long.toString(responseTime) + " ms" );
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
}
