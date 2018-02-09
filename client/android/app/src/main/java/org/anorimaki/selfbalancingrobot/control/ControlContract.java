package org.anorimaki.selfbalancingrobot.control;

import io.reactivex.Flowable;

/**
 * Created by David on 09/02/2018.
 */

public interface ControlContract {
    class JoystickMove {
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

    interface View {
        Flowable<JoystickMove> getTargets();
        void showError( String msg );
    }

    interface Presenter {
        void attach( View view );
        void detach();
    }
}
