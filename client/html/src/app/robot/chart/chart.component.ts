import { Component, OnInit, ViewChild, Input } from '@angular/core';
import { DxChartComponent, DxRangeSelectorComponent } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import DxChart from 'devextreme/viz/chart';
import ArrayStore from 'devextreme/data/array_store';
import { Range } from './range';

type MapComponent = ( e: any ) => any;

@Component( {
    selector: 'rb-char',
    templateUrl: './chart.component.html',
    styleUrls: ['./chart.component.css']
} )
export class RbChartComponent implements OnInit {
    @ViewChild( "chart" )
    private dxChart: DxChartComponent;

    @ViewChild( "rangeSelector" )
    private dxRangeSelector: DxRangeSelectorComponent;

    @Input() private y: string[];
    @Input() private x: string;

    private range: Range;
    private lastItem: any;
    private follow: boolean;
    private dataStore: ArrayStore;
    private dataSource: DataSource;

    constructor() {
        this.follow = false;
        this.range = new Range( 0, 1 );
        this.dataStore = new ArrayStore( [] );
        this.dataSource = new DataSource( {
            store: this.dataStore
        } );
        this.dataSource.paginate( false );
    }
    
    insert( items : any[] ) : void {
        if ( items.length == 0 ) {
            return;
        }
        
        for (let item of items) {
            this.dataStore.insert( item )
        }
        
        this.lastItem = items[items.length-1];
        
        this.dataSource.load().done( () => {
                let max = this.follow ? this.lastItem[this.x] : this.range.max;
                this.dxChart.instance.zoomArgument( this.range.min, max );
                this.dxRangeSelector.instance.setValue( [this.range.min, max] );
            } );
    }

    ngOnInit() {
    }

    onRangeValueChanged( e: any ) {
        this.range = new Range( e.value[0], e.value[1] );
        this.follow = ( this.range.max == this.lastItem[this.x] );
        this.dxChart.instance.zoomArgument( this.range.min, this.range.max );
    }
}
