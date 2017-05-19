import { Component, OnInit, ViewChild, Input } from '@angular/core';
import { DxChartComponent } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import { Range } from './range';

@Component( {
    selector: 'rb-char',
    templateUrl: './chart.component.html',
    styleUrls: ['./chart.component.css']
} )
export class RbChartComponent implements OnInit {

    @ViewChild("chart") 
    private dxChart: DxChartComponent;

    @Input()
    private dataSource: DataSource;
    
    @Input() 
    private series: string[];
    
    @Input() 
    private x: string;
    
    private range: Range;
    
    ngOnInit() {
        this.dataSource.paginate(false);
    }
    
    onRangeValueChanged(e: any) {
        this.range = new Range( e.value[0], e.value[1] );
        this.selectRange();
    }
    
    private selectRange() {
        this.dxChart.instance.zoomArgument( this.range.min, this.range.max );
    }
}
