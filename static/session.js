import {testbedApi} from "/static/testbed_api/testbed_api.js"


const PLOT_ID = 'waveform-container';
const PINS_INFO_ID = 'pins-info'

let pins_info = {};
let plot_data = [];

let plot_layout = {
    showlegend: false,
    margin: {l: 0, r: 0, b: 0, t: 0, pad: 0},
    xaxis: {autotick: true, ticks: 'outside', showticklabels: true, tickmode: 'auto'},
    yaxis: {autotick: true, ticks: 'outside', showticklabels: true},
    // autosize: true,
}


export function initPlot() {
    Object.assign(pins_info, JSON.parse(document.getElementById(PINS_INFO_ID).textContent));
    plot_data.length = 0;
    pins_info.pins.forEach((pin) => {
        plot_data.push({x: [], y: [], name: pin.name});
    })
    Plotly.newPlot(
        PLOT_ID,
        plot_data,
        plot_layout,
        {responsive: true}
    )
    fetchData();
}


function updatePlot() {
    Plotly.redraw(PLOT_ID)
}

window.updatePlot = updatePlot


export function adjustSize() {
    Plotly.Plots.resize(PLOT_ID)
}

window.adjustSize = adjustSize

export function uploadProgram() {
    testbedApi.upload(document.getElementById('code-input').value).then(
        (json) => {
            console.log(json);
            let error_elem = document.getElementById('upload-error-text');
            console.log(error_elem);
            if (json !== {}) {
                error_elem.innerText = ('error' in json) ? json.error : 'Unknown error';
            } else {
                error_elem.innerText = 'ok';
            }
        }
    )
}

window.uploadProgram = uploadProgram;


export function setInput(index, state) {
    let inputs = [];
    for (let pin_i = 0; pin_i < pins_info.pins.length; ++pin_i) {
        let input_displays = document.querySelectorAll(`.input-state[port-index="${pin_i}"]`);
        if (input_displays.length > 0) {
            inputs.push(input_displays[0].checked);
        }
    }
    inputs[index] = !!state;
    testbedApi.setInputs(inputs);
}

window.setInput = setInput


export function fetchData() {
    // for (let t = 0; t < 100; ++t) {
    //     for (let i = 0; i < plot_data.length; ++i) {
    //         let v = (t >> i) & 0b1 ? 1 : 0;
    //         let y_v = v * 0.8 + 0.1;
    //         plot_data[i].x.push(t);
    //         plot_data[i].y.push(i + y_v);
    //         plot_data[i].x.push(t + 1);
    //         plot_data[i].y.push(i + y_v);
    //     }
    // }
    testbedApi.getStateHistory().then((history) => {
        for (let pin_i = 0; pin_i < pins_info.pins.length; ++pin_i) {
            let wave_data = plot_data[pin_i];
            wave_data.x.length = 0;
            wave_data.y.length = 0;
            for (let i = 0; i < history.time.length; ++i) {
                let ts = history.time[i];
                let value = history.states[i][pin_i];
                let y = pin_i + 0.1 + value * 0.8;
                if (i > 0) {
                    wave_data.x.push(ts);
                    wave_data.y.push(wave_data.y[wave_data.y.length - 1]);
                }
                wave_data.x.push(ts);
                wave_data.y.push(y);
            }
            let curValue = history.states.length > 0 ? history.states[history.states.length - 1][pin_i] : null;
            let input_displays = document.querySelectorAll(`.input-state[port-index="${pin_i}"]`);
            if (input_displays.length > 0) {
                input_displays[0].checked = (curValue === 1);
            }
            let state_displays = document.querySelectorAll(`.output-state[port-index="${pin_i}"]`);
            if (state_displays.length > 0) {
                state_displays[0].setAttribute(
                    'value',
                    curValue === null ? 'None' : curValue.toString());
            }
        }
        updatePlot();
    })
}

window.plot_data = plot_data;
window.fetchData = fetchData;

document.addEventListener('DOMContentLoaded', initPlot);
document.addEventListener('DOMContentLoaded', () => {
    setInterval(fetchData, 500);
});
