package org.anorimaki.selfbalancingrobot.robot;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;

/**
 * Created by david.osorio on 01/02/2018.
 */

public class Targets {
    @JsonProperty(value = "speed")
    private int speed;

    @JsonProperty(value = "heading")
    private int heading;

    public Targets(int speed, int heading ) {
        this.speed = speed;
        this.heading = heading;
    }
}
