import { Component, OnInit, ViewChild, Input, Output, EventEmitter } from '@angular/core';
import { AmChartsService } from "@amcharts/amcharts3-angular";
import { MdSlideToggleChange } from "@angular/material/material";

type YOption = { label:string, checked: boolean, color: number };

@Component( {
    selector: 'rb-char',
    templateUrl: './chart.component.html',
    styleUrls: ['./chart.component.css']
} )
export class RbChartComponent implements OnInit {
    private static COLORS: string[] = ["#b5030d", "#05F30d", "#1503Fd"];

    @Input() private y: string[];
    @Input() private x: string;
    
    @Output() enableChange = new EventEmitter<boolean>(); 

    private yOptions: YOption[];
    private chart: any;
    private lastItem: any;
    @Input() enabled: boolean;

    constructor(private amCharts: AmChartsService) {
        this.enabled = true;
    }

    ngOnInit(): void {
        this.yOptions = this.y.map( (label,index) => ({ label: label, checked: true, color:index } as YOption) ) ;
       
        this.chart = this.amCharts.makeChart( "chartdiv", {
            type: "xy",
            dataProvider: [],
            graphs: this.getGraphs(),
            chartCursor: {
                fullWidth:true,
                cursorAlpha:0.1,
                limitToGraph: "g0"
            },
            chartScrollbar: {
                scrollbarHeight: 30,
                color: "#AAAAAA",
                autoGridCount: true,
                graph: "g0"
            },
            mouseWheelZoomEnabled:true,
            hideYScrollbar: true
        });
    }
    
    ngOnDestroy(): void {
        this.amCharts.destroyChart( this.chart );
    }
    
    insert( items : any[] ) : void {
        this.chart.dataProvider.push( ...items );
        this.chart.validateData();
    }
    
    labelSelectionChanged( event ): void {
        this.yOptions.
            filter( option => (option.label == event.source.value) )[0].checked = event.checked;
        this.chart.graphs = this.getGraphs();
        this.chart.validateData();
    }
    
    private enableChanged( event: MdSlideToggleChange ): void {
        this.enableChange.emit( event.checked );
    }
    
    private getGraphs(): any[] {
        return this.yOptions.
            filter( option => option.checked ).
            map( (option, index) =>  ({
                        id: "g"+ index, 
                        title: option.label,
                        xField: this.x,
                        yField: option.label,
                        lineThickness: 1,
                        lineColor: RbChartComponent.COLORS[option.color],
                        balloonText: "[[category]]<br><b><span style='font-size:12px;'>[[value]]</span></b>"
                    }) );
    }
}
