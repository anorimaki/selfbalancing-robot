import { Component, OnInit, OnDestroy, ViewChild, 
		Input, Output, EventEmitter, ChangeDetectorRef } from '@angular/core';
import { AmChartsService, AmChart } from "@amcharts/amcharts3-angular";
import { MatSlideToggleChange } from "@angular/material/material";

type Serie = { label: string, field: string, checked: boolean, color: number, type: number };

@Component( {
    selector: 'rb-char',
    templateUrl: './chart.component.html',
    styleUrls: ['./chart.component.css']
} )
export class RbChartComponent implements  OnDestroy {
    private static readonly MAX_ENTIES = 900; 
    private static readonly COLORS: string[] = ["#b5030d", "#05F3Fd", "#1503Fd", "#F30300", "#00FF00"];

    @Input() private y: Serie[];
    @Input() private x: string;
    
    @Output() enableChange = new EventEmitter<boolean>(); 

    private chart: AmChart;
    @Input() enabled: boolean;

	constructor( private amCharts: AmChartsService,
				private ref: ChangeDetectorRef ) {
        this.enabled = true;
    }

    ngAfterViewInit(): void {
        this.y = this.y.map( (input,i) => ({ 
                label: input.label,
                field: input.field, 
                type: (input.type!=null) ? input.type : 1000+i, 
                checked: (input.checked!=null) ? input.checked : true, 
                color: (input.color!=null) ? input.color : i, 
            } as Serie) ) ;
       
        let charConfig = {
                type: "xy",
                dataProvider: [],
                processTimeout: 100,
             /*   legend: {
                    useGraphSettings: true,
                    divId: "chartlegend",
                    maxColumns: 1,
                    align: "center"
                },
              */  chartCursor: {
                    cursorAlpha:0.3,
                    cursorPosition: "mouse"
                },
                chartScrollbar: {},
                mouseWheelZoomEnabled:true,
                hideYScrollbar: true
            };
        
        this.configSeries( charConfig );
        
        this.chart = this.amCharts.makeChart( "chartdiv", charConfig );
		this.chart.validateNow(true);
		this.ref.detectChanges();
    }
    
    ngOnDestroy(): void {
		if (this.chart) {
			this.amCharts.destroyChart( this.chart );
		}
    }
    
    insert( items : any[] ) : void {
		if ( items.length > RbChartComponent.MAX_ENTIES ) {
			items.splice( 0, items.length-RbChartComponent.MAX_ENTIES );
		}
        let itemsToRemove = (this.chart.dataProvider.length + items.length) - RbChartComponent.MAX_ENTIES;
        if ( itemsToRemove > 0 ) {
            this.chart.dataProvider.splice( 0, itemsToRemove );
        }
        this.chart.dataProvider.push( ...items );
        if ( (items.length > 0) && (this.chart.graphs[0].xAxis != null) ) {
            this.amCharts.updateChart( this.chart, () => {
                this.chart.graphs[0].xAxis.strictMinMax = true;
                this.chart.graphs[0].xAxis.minimum=this.chart.dataProvider[0].index;
                this.chart.graphs[0].xAxis.maximum=this.chart.dataProvider[0].index + RbChartComponent.MAX_ENTIES;
                this.chart.validateNow();
            }); 
        }
        this.chart.validateData();
    }
    
    labelSelectionChanged( event ): void {
        this.y.filter( option => (option.field == event.source.value) )[0].checked = event.checked;
        this.configSeries( this.chart );
        this.chart.validateData();
    }
    
    private enableChanged( event: MatSlideToggleChange ): void {
        this.enableChange.emit( event.checked );
    }
    
    private configSeries( char: any ): void {
        let series = this.y.filter( option => option.checked );
        
        let graphs = [];
        let graphTypes = new Set();
        series.forEach((serie, index) => {
            let graph = {
                    id: "g"+ index,
                    yAxis: "v" + serie.type,
                    title: serie.label,
                    xField: this.x,
                    yField: serie.field,
                    lineThickness: 1,
           //         lineColor: RbChartComponent.COLORS[serie.color],
                    bullet: "round",
                    bulletSize: 1,
                    minBulletSize: 1,
                    balloonText: "[[x]]<br><b><span style='font-size:12px;'>[[y]]</span></b>"
                };
            graphs.push( graph );
            graphTypes.add( serie.type );
        });
       
        char.graphs = graphs;
        char.valueAxes = Array.from(graphTypes).map( (t,index) => ({
                id: "v" + t,
                position: "left",
                offset: index * 60
            }) );
    }
}
