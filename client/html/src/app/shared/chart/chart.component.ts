import { Component, OnInit, OnDestroy, ViewChild, 
		Input, Output, EventEmitter, ChangeDetectorRef,
		ChangeDetectionStrategy } from '@angular/core';
import { MatSlideToggleChange } from "@angular/material/material";
declare var CanvasJS: any;

export interface RbChartSerie { 
	label: string;
	field: string;
	checked?: boolean;
	color?: number; 
	type: number;
};

@Component( {
    selector: 'rb-char',
    templateUrl: './chart.component.html',
	styleUrls: ['./chart.component.css']
} )
export class RbChartComponent implements  OnDestroy {
    private static readonly MAX_ENTIES = 900; 
    private static readonly COLORS: string[] = ["#b5030d", "#05F3Fd", "#1503Fd", "#F30300", "#00FF00"];

	@Input() 
	private y: RbChartSerie[];

	@Input() 
	private x: string;

	@Input() 
	private enabled: boolean;
    
	@Output("enable") 
	private enableEvent = new EventEmitter<void>(); 

	@Output("disable") 
	private disableEvent = new EventEmitter<void>(); 

	private chart: any;
	private chartOptions: any;
	private seriesData: any[][];

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
				xValueFormatString: "#.###",
				axisYIndex: serie.type,
				dataPoints: this.seriesData[index],
				color: RbChartComponent.COLORS[index]
			});
			axisY[serie.type] = {
				title: axisY[serie.type] ?
							(axisY[serie.type].title + ', ' + serie.label) :
							serie.label,
				titleFontWeight: "bold"
			};
		});
		
		this.chartOptions = {
			zoomEnabled: true,
			axisY: axisY,
			axisX: {
				suffix: " s",
		  	},  
			data: charData
		};

		this.chart = new CanvasJS.Chart("chartdiv", this.chartOptions);
	}

    private labelSelectionChanged = ( index, event ) => {
		this.chartOptions.data[index].visible = event.checked;
		this.chart.render();
	}
    
    private onEnableChanged( event: MatSlideToggleChange ): void {
		if ( event.checked ) {
			this.enableEvent.emit();
		}
		else {
			this.disableEvent.emit();
		}
    }
}
