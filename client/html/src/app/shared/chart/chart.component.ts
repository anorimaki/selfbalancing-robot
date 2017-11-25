import { Component, OnInit, OnDestroy, ViewChild, 
		Input, Output, EventEmitter, ChangeDetectorRef } from '@angular/core';
import { MatSlideToggleChange } from "@angular/material/material";
declare var CanvasJS: any;

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

	private chart: any;
	private chartOptions: any;
	private seriesData: any[][];
    @Input() enabled: boolean;

	constructor() {
        this.enabled = true;
    }

    ngOnInit(): void {
		this.seriesData = [];
		this.y.forEach( (serie, index) => {
			this.seriesData.push([]);
		});

		this.buildChar();
    }
    
    ngOnDestroy(): void {
    }
    
    insert( items : any[] ) : void {
		if ( items.length > RbChartComponent.MAX_ENTIES ) {
			items.splice( 0, items.length-RbChartComponent.MAX_ENTIES );
		}

		this.seriesData.forEach( (values) => {
			let itemsToRemove = (values.length + items.length) - RbChartComponent.MAX_ENTIES;
			if ( itemsToRemove > 0 ) {
				values.splice( 0, itemsToRemove );
			}
		});

		items.forEach( item => {
			let x = item[this.x];
			this.y.forEach( (serie, index) => {
				let points = this.seriesData[index]
				points.push( {
					x: x, 
					y: item[serie.field]
				});
			});
		});

		this.chart.render();
	}
	
	private buildChar(): void {
		let charData = [];
		let axisY = [];
		this.y.forEach( (serie, index) => {
			charData.push({
				type: "line",
				visible: true,
				showInLegend: false,
				legendText: serie.label,
				axisYIndex: serie.type,
				dataPoints: this.seriesData[index],
				color: RbChartComponent.COLORS[index]
			});
			axisY[serie.type] = {};
		});
		
		this.chartOptions = {
			/*legend: {
				horizontalAlign: "right", // "center" , "right"
				verticalAlign: "center",  // "top" , "bottom"
				fontSize: 15,
				itemclick: this.kk
			},*/
			zoomEnabled: true,
			axisY: axisY,
			data: charData
		};

		this.chart = new CanvasJS.Chart("chartdiv", this.chartOptions);
	}


    private labelSelectionChanged = ( index, event ) => {
		this.chartOptions.data[index].visible = event.checked;
		this.chart.render();
	//	this.buildChar();
	}
    
    private enableChanged( event: MatSlideToggleChange ): void {
        this.enableChange.emit( event.checked );
    }
}
