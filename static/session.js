import {testbedApi} from "/static/testbed_api/testbed_api.js"


const PLOT_ID = 'waveform-container';
const PINS_INFO_ID = 'pins-info'

let pinsInfo = {};
let plotData = [];

let plotLayout = {
    title: "Waveform",
    showlegend: false,
    margin: {l: 75, r: 0, b: 40, t: 25, pad: 0},
    xaxis: {
        title: "TIME (S)",
        range: [0, 10],
        rangemode: 'nonnegative',
        autotick: true, tickmode: 'auto',
        ticks: 'outside', showticklabels: true,
        autorange: true, autorangeoptions: {
            clipmin: null,
        }
    },
    yaxis: {
        fixedrange: true,
        rangemode: 'tozero',
        range: [0, null],
        ticks: 'outside', showticklabels: true,
        tickmode: 'array',
        tickvals: [], ticktext: [],
        showgrid: false,
        minor: {
            ticks: 'inside', showticklabels: true,
            tickmode: 'array',
            tickvals: [],
            showgrid: true,
            gridcolor: 'rgb(0,0,0)',
        }
    },
    // autosize: true,
}


export function initPlot() {
    Object.assign(pinsInfo, JSON.parse(document.getElementById(PINS_INFO_ID).textContent));
    const nPins = pinsInfo.pins.length;
    plotData.length = 0;
    plotLayout.yaxis.tickvals.length = 0;
    plotLayout.yaxis.ticktext.length = 0;
    plotLayout.yaxis.minor.tickvals.length = 0;
    for (const [i, pin] of pinsInfo.pins.entries()) {
        plotData.push({x: [], y: [], name: pin.name, mode: 'lines'});
        plotLayout.yaxis.tickvals.push(i + 0.5);
        plotLayout.yaxis.ticktext.push(pin.name);
        plotLayout.yaxis.minor.tickvals.push(i);
    }
    plotLayout.yaxis.minor.tickvals.push(nPins);
    plotLayout.yaxis.range[1] = nPins + 0.03;
    Plotly.newPlot(
        PLOT_ID,
        plotData,
        plotLayout,
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
    const button = document.getElementById('upload-button');
    const loader = document.getElementById('upload-loader');
    const uploadResult = document.getElementById('upload-result');
    const errorDisplay = document.getElementById('upload-error-text');
    button.disabled = true;
    loader.style.visibility = 'visible';
    uploadResult.innerText = '';
    errorDisplay.value = '';
    // errorDisplay.style.visibility = 'hidden';
    testbedApi.upload(document.getElementById('code-input').value).then(
        (json) => {
            console.log(json);
            button.disabled = false;
            loader.style.visibility = 'hidden';
            // errorDisplay.style.visibility = 'visible';
            if (json.error) {
                uploadResult.innerText = 'ERROR';
                errorDisplay.value = json.error;
            } else {
                uploadResult.innerText = 'DONE';
                errorDisplay.value = json.log;
            }
        }
    )
}

window.uploadProgram = uploadProgram;


function _setPinControlEnabled(enabled) {
    const container = document.getElementById('pin-controls-container');
    if (enabled) {
        container.classList.remove('disabled-gray');
    } else {
        container.classList.add('disabled-gray');
    }
    document.querySelectorAll('.input-state, .output-state')
        .forEach((state) => {
            state.disabled = !enabled;
        });
}


export function setMonitorState(enable) {
    const monitorStateInput = document.getElementById('monitor-state');
    const monitorStateContainer = document.getElementById('monitor-state-container');
    console.log("Set state " + enable);
    monitorStateInput.checked = !enable;
    monitorStateInput.disabled = true;
    monitorStateContainer.classList.add('disabled-gray');
    _setPinControlEnabled(false);
    testbedApi.setMonitorState(enable).then((result) => {
        setTimeout(() => {
            monitorStateInput.disabled = false;
            monitorStateContainer.classList.remove('disabled-gray');
            if (result.ok === true) {
                _setPinControlEnabled(enable);
            }
        }, 1000);
        if (result.ok === true) {
            if (enable) {
                historyPastI = 0;
                const traceIs = [];
                let i = 0;
                for (const trace of plotData) {
                    trace.x.length = 0;
                    trace.y.length = 0;
                    traceIs.push(i++);
                }
                Plotly.extendTraces(PLOT_ID, {}, traceIs);
                // Plotly.deleteTraces(PLOT_ID, traceIs);
            }
            monitorStateInput.checked = enable;
        }
        if (result.error) {
            console.error(result.error);
        }
    });
}

window.setMonitorState = setMonitorState;


export function setInputState(index, state) {
    const inputs = [];
    const displays = []
    let changedI = null;
    for (const [i, pin] of pinsInfo.input.indexed.entries()) {
        if (pin !== null) {
            const inputDisplay = document.querySelectorAll(
                `.input-state[port-index="${pin.index}"]`)[0];
            displays.push(inputDisplay);
            if (pin.index === index) {
                changedI = i;
                inputs.push(!!state);
                inputDisplay.disabled = true;
                inputDisplay.checked = !state;
            } else {
                inputs.push(inputDisplay.checked);
            }
        } else {
            inputs.push(null);
        }
    }
    testbedApi.setInputs(inputs).then((json) => {
        for (const inputDisplay of displays) {
            inputDisplay.disabled = false;
        }
        displays[changedI].checked = state;
    });
}

window.setInputState = setInputState

let startTs = Date.now() / 1000;
let endTs = Date.now() / 1000;
let historyPastI = 0;


export function scrollPlot() {
    // endTs = Date.now() / 1000;
    Plotly.relayout(PLOT_ID, {'xaxis.autorangeoptions.clipmin': endTs - startTs - 10});
}


export function fetchData() {
    testbedApi.getStateHistory().then((history) => {
        if (!history.error && history.time.length > 0) {
            startTs = history.time[0];
            endTs = history.time[history.time.length - 1];
            // plotLayout.xaxis.autorangeoptions.clipmin = endTs - startTs - 10;
            const updateIs = [];
            const updateXs = [];
            const updateYs = [];
            for (let pinI = 0; pinI < pinsInfo.input.pins.length; ++pinI) {
                const waveData = plotData[pinI];
                const pin = pinsInfo.input.pins[pinI];

                const updateX = [];
                const updateY = [];
                updateIs.push(pinI);
                updateXs.push(updateX);
                updateYs.push(updateY);

                for (let i = historyPastI; i < history.time.length; ++i) {
                    const ts = history.time[i] - history.time[0];
                    const value = history.states[i][pin.index];
                    const y = pinI + 0.1 + value * 0.8;
                    if (i > 0) {
                        updateX.push(ts);
                        if (i === historyPastI) {
                            updateY.push(waveData.y[waveData.y.length - 1]);
                        } else {
                            updateY.push(updateY[updateY.length - 1]);
                        }
                    }
                    updateX.push(ts);
                    updateY.push(y);
                }
                const curValue = history.states.length > 0 ? history.states[history.states.length - 1][pin.index] : null;
                const inputDisplay = document.querySelectorAll(`.input-state[port-index="${pin.index}"]`)[0];
                if (inputDisplay !== undefined) {
                    inputDisplay.checked = (curValue === 1);
                    // inputDisplay.disabled = !document.getElementById('monitor-state').checked;
                }
                const stateDisplay = document.querySelectorAll(`.output-state[port-index="${pin.index}"]`)[0];
                if (stateDisplay !== undefined) {
                    stateDisplay.setAttribute(
                        'value',
                        curValue === null ? 'None' : curValue.toString());
                }
            }
            // Adding trace with only one zero may result in axis becoming reverse because of auto range.
            if (updateXs[0].length >= 2 || updateXs[0][0] !== 0) {
                Plotly.extendTraces(PLOT_ID, {x: updateXs, y: updateYs}, updateIs);
                historyPastI = history.time.length;
            }
            scrollPlot();
        }
        // updatePlot();
    })
}

window.plotLayout = plotLayout;
window.plotData = plotData;
window.fetchData = fetchData;

document.addEventListener('DOMContentLoaded', initPlot);
document.addEventListener('DOMContentLoaded', () => {
    // const feed_promise = (async () => {
    //     const img = document.getElementById('camera-feed')
    //     const body = await testbedApi.getCameraFeed();
    //     let done = false;
    //     console.log(body);
    //     for await (const chunk of stream) {
    //         console.log(chunk);
    //     }
    //     // while (!done) {
    //     //     let value = await reader.read();
    //     //     console.log(value);
    //     //     // img.src = 'data:image/jpeg;base64,' + btoa('your-binary-data');
    //     // }
    //     console.log("Ended");
    // })();
    testbedApi.getMonitorState().then((response) => {
        const monitorState = document.getElementById('monitor-state');
        let running = response === true;
        monitorState.checked = running;
        monitorState.disabled = false;
        document.getElementById('monitor-state-container').classList.remove('disabled-gray');
        _setPinControlEnabled(running);
    });
    // setInterval(scrollPlot, 100);
    window.intervalFetchData = setInterval(fetchData, 1500);
    // feed_promise.then(() => console.log("done"));
});
