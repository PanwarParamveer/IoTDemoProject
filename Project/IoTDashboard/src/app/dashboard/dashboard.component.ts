import { Component, OnInit } from '@angular/core';
import { AngularFireDatabase } from '@angular/fire/database';
import * as Highcharts from 'highcharts';
@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css'],
})
export class DashboardComponent implements OnInit {
  Bulbstate: any;
  img: any;
  requestState: number;
  highcharts = Highcharts;

  tempData: any = [];
  tempTimeStamp: any = [];
  items: any;
   data: { name: string; data: any; }[];
   chartOptions: { chart: { type: string; }; title: { text: string; }; xAxis: { categories: any; }; yAxis: { title: { text: string; }; }; series: { name: string; data: any; }[]; };

  constructor(private db: AngularFireDatabase) {}

  ngOnInit(): void {
    this.img = 'assets/pic_bulboff.gif';
    this.db
      .object('BulbState')
      .valueChanges()
      .subscribe((e: any) => {
        this.Bulbstate = e.State;
      });

    this.db
      .list('/SensorData')
      .valueChanges()
      .subscribe((e) => {
        console.log(e);
        this.tempData=[];
        this.tempTimeStamp=[];
        e.forEach((element) => {
          var data = element;
          this.tempData.push(element['Temperature']);
          this.tempTimeStamp.push( new Date(element['TimeStamp']*1000).toLocaleTimeString());
        });

        this.data = [
         {
           name: 'Room Temperature',
           data: this.tempData,
         },
       ];
     
       this.chartOptions = {
         chart: {
           type: 'spline',
         },
         title: {
           text: 'Temperature Sensor',
         },
         xAxis: {
           categories: this.tempTimeStamp,
         },
         yAxis: {
           title: {
             text: 'Temperature',
           },
         },
         series: this.data,
       };


      });

    //    //   for (let index = 0; index < e.length; index++) {
    //    //     this.tempData.push(e[index].Temperature);
    //    //     this.tempTimeStamp.push(e[index].TimeStamp);
    //    //   }
  }

  OnOffBulb() {
    this.requestState = this.Bulbstate === 1 ? 0 : 1;

    this.db.object('BulbState').update({ State: this.requestState });
  }


}
